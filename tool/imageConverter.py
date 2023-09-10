# 导入Pillow库、re模块和os模块
from PIL import Image
import re
import os

# 由用户手动输入文件名
file_name = input("请输入图片文件名：")

# 检查文件是否存在
while not os.path.exists(file_name):
    print("文件不存在，请重新输入。")
    file_name = input("请输入图片文件名：")

# 检查文件名是否有效
while not file_name.endswith((".png", ".jpg", ".jpeg", ".bmp", ".gif")):
    print("无效的图片格式，请重新输入。")
    file_name = input("请输入图片文件名：")

# 打开原始图片
original_image = Image.open(file_name)

# 调整图片大小为240x240
resized_image = original_image.resize((240, 240))

# 输出文件名为原文件名_resized.jpg，如果有ASCII以外的字符，替换为_
output_name = re.sub("[^\x00-\x7F]", "_", file_name.split(".")[0]) + "_resized.jpg"

# 保存为JPG格式
resized_image.save(output_name)
