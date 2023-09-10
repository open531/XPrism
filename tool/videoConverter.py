# 导入OpenCV库
import cv2

# 导入os库，用于处理文件名和路径
import os

# 定义一个函数，用于替换文件名中的非ASCII字符
def replace_non_ascii(filename):
    # 创建一个空字符串，用于存储替换后的文件名
    new_filename = ""

    # 循环遍历文件名中的每一个字符
    for char in filename:
        # 如果字符是ASCII字符，保留原样
        if ord(char) < 128:
            new_filename += char
        # 如果字符是非ASCII字符，替换为下划线
        else:
            new_filename += "_"

    # 返回替换后的文件名
    return new_filename

# 让用户输入要转换的视频文件名
input_video = input("请输入要转换的视频文件名：")

# 检查输入的文件名是否有效、是否存在
# 如果文件名为空，或者文件不存在，或者文件不是视频格式，打印错误信息，并退出程序
if not input_video or not os.path.exists(input_video) or not input_video.endswith((".mp4", ".avi", ".mov", ".mkv")):
    print("无效的视频文件名，请重新输入。")
    exit()

# 将输入的视频文件名中的非ASCII字符进行替换
input_video = replace_non_ascii(input_video)

# 根据输入的视频文件名，生成输出的视频文件名
# 输出文件名为原文件名_resize.mjpg
output_video = input_video[:-4] + "_resize.mjpg"

# 创建一个VideoCapture对象，用于读取输入视频
cap = cv2.VideoCapture(input_video)

# 获取输入视频的宽度、高度和帧率
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = cap.get(cv2.CAP_PROP_FPS)

# 计算输入视频的总帧数
frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

# 创建一个VideoWriter对象，用于写入输出视频
# 使用XVID编码器，输出为avi格式
# 将输出视频的帧率设为15
fourcc = cv2.VideoWriter_fourcc(*"XVID")
out = cv2.VideoWriter(output_video + ".avi", fourcc, 15, (240, 240))

# 定义一个变量，用于记录当前处理的帧数
current_frame = 0

# 循环读取输入视频的每一帧
while cap.isOpened():
    # 读取一帧图像
    ret, frame = cap.read()

    # 如果读取成功，继续处理
    if ret:
        # 将图像缩放到240x240大小
        frame = cv2.resize(frame, (240, 240))

        # 将图像写入输出视频
        out.write(frame)

        # 更新当前处理的帧数
        current_frame += 1

        # 打印进度信息
        print(f"处理第{current_frame}帧 / 共{frame_count}帧")

    # 如果读取失败，退出循环
    else:
        break

# 释放VideoCapture和VideoWriter对象
cap.release()
out.release()

# 使用ffmpeg工具，将输出视频的格式转换为mjpg
os.system(f"ffmpeg -i {output_video}.avi {output_video}")
