#include "led.h"
#include <Arduino.h>
#include "system.h"

void Pixel::init()
{
	FastLED.addLeds<WS2812, LED_PIN, GRB>(buffers, LED_NUM);
	FastLED.setBrightness(200);
}

Pixel &Pixel::setRGB(int r, int g, int b)
{
	for (int pos = 0; pos < LED_NUM; ++pos)
	{
		buffers[pos] = CRGB(r, g, b);
	}
	FastLED.show();

	return *this;
}

Pixel &Pixel::setHVS(uint8_t ih, uint8_t is, uint8_t iv)
{
	for (int pos = 0; pos < LED_NUM; ++pos)
	{
		buffers[pos].setHSV(ih, is, iv);
	}
	FastLED.show();

	return *this;
}

Pixel &Pixel::fill_rainbow(int min_r, int max_r,
						   int min_g, int max_g,
						   int min_b, int max_b)
{
	fill_gradient(buffers, 0, CHSV(50, 255, 255), 29, CHSV(150, 255, 255), SHORTEST_HUES);
	FastLED.show();

	return *this;
}

Pixel &Pixel::setBrightness(float duty)
{
	duty = constrain(duty, 0, 1);
	FastLED.setBrightness((uint8_t)(255 * duty));
	FastLED.show();

	return *this;
}

LED_RUN_MODE run_mode = RUN_MODE_NONE;
LEDParam g_led;
LEDStatus led_status;
BaseType_t taskRgbReturned;
TaskHandle_t handleLed = NULL;
TimerHandle_t xTimer_led = NULL;

void led_timerHandler(TimerHandle_t xTimer);
void led_taskHandler(void *parameter);
static void hsvModeChange(void);
static void rgbModeChange(void);
static void onceChange(void);
static void count_cur_brightness(void);

bool set_led_and_run(LEDParam *led_setting, LED_RUN_MODE mode)
{
	if (RUN_MODE_NONE <= mode)
	{
		// 创建失败
		return false;
	}

	if (run_mode != mode)
	{
		// 运行模式发生变化
		led_stop();
		run_mode = mode;
	}

	g_led = *led_setting;

	// 拷贝数据
	if (LED_MODE_RGB == g_led.mode)
	{
		led_status.current_r = g_led.min_value_r;
		led_status.current_g = g_led.min_value_g;
		led_status.current_b = g_led.min_value_b;
		led_status.current_brightness = g_led.min_brightness;
		led_status.pos = 0;
	}
	else if (LED_MODE_HSV == g_led.mode)
	{
		led_status.current_h = g_led.min_value_h;
		led_status.current_s = g_led.min_value_s;
		led_status.current_v = g_led.min_value_v;
		led_status.current_brightness = g_led.min_brightness;
		led_status.pos = 0;
	}

	// 选择启动两种运行模式
	if (RUN_MODE_TIMER == run_mode)
	{
		if (NULL != xTimer_led)
		{
			xTimerStop(xTimer_led, 0);
			xTimer_led = NULL;
		}
		xTimer_led = xTimerCreate("led_timerHandler",
								  g_led.time / portTICK_PERIOD_MS,
								  pdTRUE, (void *)0, led_timerHandler);
		xTimerStart(xTimer_led, 0); // 开启定时器
	}
	else if (RUN_MODE_TASK == run_mode)
	{
		if (NULL == handleLed)
		{
			taskRgbReturned = xTaskCreate(
				led_taskHandler,
				"led_taskHandler",
				8 * 128, // 实际上 7*128就够用
				(void *)&g_led.time,
				TASK_RGB_PRIORITY,
				&handleLed);
			if (taskRgbReturned != pdPASS)
			{
				return false;
			}
		}
	}
	return true;
}

void led_timerHandler(TimerHandle_t xTimer)
{
	onceChange();
}

void led_taskHandler(void *parameter)
{
	int *ms = (int *)parameter; // 控制时间
	for (;;)
	{
		onceChange();
		vTaskDelay(*ms);

		// if (pdTRUE == xSemaphoreTake(lvgl_mutex, portMAX_DELAY))
		// {
		//     lv_task_handler();
		//     xSemaphoreGive(lvgl_mutex);
		// }
	}
}

static void onceChange(void)
{
	if (LED_MODE_RGB == g_led.mode)
	{
		rgbModeChange();
	}
	else if (LED_MODE_HSV == g_led.mode)
	{
		hsvModeChange();
	}
}

static void hsvModeChange(void)
{
	// HSV色彩的控制
	led_status.current_h += g_led.step_h;
	if (led_status.current_h >= g_led.max_value_h)
	{
		g_led.step_h = (-1) * g_led.step_h;
		led_status.current_h = g_led.max_value_h;
	}
	else if (led_status.current_h <= g_led.min_value_h)
	{
		g_led.step_h = (-1) * g_led.step_h;
		led_status.current_h = g_led.min_value_h;
	}

	led_status.current_s += g_led.step_s;
	if (led_status.current_s >= g_led.max_value_s)
	{
		g_led.step_s = (-1) * g_led.step_s;
		led_status.current_s = g_led.max_value_s;
	}
	else if (led_status.current_s <= g_led.min_value_s)
	{
		g_led.step_s = (-1) * g_led.step_s;
		led_status.current_s = g_led.min_value_s;
	}

	led_status.current_v += g_led.step_v;
	if (led_status.current_v >= g_led.max_value_v)
	{
		g_led.step_v = (-1) * g_led.step_v;
		led_status.current_v = g_led.max_value_v;
	}
	else if (led_status.current_v <= g_led.min_value_v)
	{
		g_led.step_v = (-1) * g_led.step_v;
		led_status.current_v = g_led.min_value_v;
	}

	// 计算当前背光值
	count_cur_brightness();

	// 设置HSV状态
	led.setHVS(led_status.current_h,
			   led_status.current_s,
			   led_status.current_v)
		.setBrightness(led_status.current_brightness);
}

static void rgbModeChange(void)
{
	// RGB色彩的控制
	if (0 == led_status.pos) // 控制到R
	{
		led_status.current_r += g_led.step_r;
		if (led_status.current_r >= g_led.max_value_r && g_led.step_r > 0)
		{
			led_status.pos = 1;
			led_status.current_r = g_led.max_value_r;
		}
		else if (led_status.current_r <= g_led.min_value_r && g_led.step_r < 0)
		{
			g_led.step_r = (-1) * g_led.step_r;
			led_status.current_r = g_led.min_value_r;
		}
	}
	else if (1 == led_status.pos) // 控制到G
	{
		led_status.current_g += g_led.step_r;
		if (led_status.current_g >= g_led.max_value_g && g_led.step_r > 0)
		{
			led_status.pos = 2;
			led_status.current_g = g_led.max_value_g;
		}
		else if (led_status.current_g <= g_led.min_value_g && g_led.step_r < 0)
		{
			led_status.pos = 0;
			led_status.current_g = g_led.min_value_g;
		}
	}
	else if (2 == led_status.pos) // 控制到B
	{
		led_status.current_b += g_led.step_r;
		if (led_status.current_b >= g_led.max_value_b && g_led.step_r > 0)
		{
			g_led.step_r = (-1) * g_led.step_r;
			led_status.current_b = g_led.max_value_b;
		}
		else if (led_status.current_b <= g_led.min_value_b && g_led.step_r < 0)
		{
			led_status.pos = 1;
			led_status.current_b = g_led.min_value_b;
		}
	}

	// 计算当前背光值
	count_cur_brightness();

	// 设置RGB状态
	led.setRGB(led_status.current_r,
			   led_status.current_g,
			   led_status.current_b)
		.setBrightness(led_status.current_brightness);
}

static void count_cur_brightness(void)
{
	// 背光的控制
	led_status.current_brightness += g_led.brightness_step;
	if (led_status.current_brightness >= g_led.max_brightness)
	{
		led_status.current_brightness = g_led.max_brightness;
		g_led.brightness_step = (-1) * g_led.brightness_step;
	}
	else if (led_status.current_brightness <= g_led.min_brightness)
	{
		led_status.current_brightness = g_led.min_brightness;
		g_led.brightness_step = (-1) * g_led.brightness_step;
	}
}

void led_stop(void)
{
	if (RUN_MODE_TIMER == run_mode &&
		NULL != xTimer_led)
	{
		xTimerStop(xTimer_led, 0);
		xTimer_led = NULL;
	}

	if (RUN_MODE_TASK == run_mode &&
		NULL != handleLed)
	{
		vTaskDelete(handleLed);
		handleLed = NULL;
	}
}