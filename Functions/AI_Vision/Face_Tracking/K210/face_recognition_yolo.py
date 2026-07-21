'''
@file:    face_recognition_yolo.py
@company: Hiwonder
@author:  CuZn
@date:    2023-08-22
@description: 人脸识别
'''

import hiwonder
import sensor  #导入相机模块
import image   #导入图像处理模块
import time    #导入时间模块
import lcd     #导入 LCD 屏幕模块
#加载KPU模块
from maix import KPU

##############################################################
# 通过i2c向主机发送数据
# @parm1：寄存器地址
# @parm2：需要传输的数据
# i2c.set_reg_value(parm1, bytearray([22,33,44]))
##############################################################
# 通过i2c从主机接收数据
# @parm1：主机发送的寄存器地址
# @parm2：需要获取的字节数 最大为100
# ret = i2c.get_reg_value(50, 6)
##############################################################


i2c = hiwonder.hw_slavei2c()

#初始化LCD
lcd.init()
#以下是初始化传感器
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 100)

#设备地址：0x32
#par1：第一个不用管
#par2：寄存器地址
#par3：传输的数据
#slaveiic.set_reg_value(0, 0x00, bytearray([1, 22, 0, 54, 0, 86, 0, 32, 0]))
#slaveiic.set_reg_value(0, 0x10, bytearray([2, 43, 0, 32, 0, 22, 0, 16, 0]))

face_data = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])
#slaveiic.set_reg_value(0, 0xB0, bytearray([5]))
#slaveiic.set_reg_value(0, 0xC0, bytearray([6]))

#帧率时钟
clock = time.clock()

#创建图像对象
img_obj = image.Image(size=(320,256))

#创建锚框的尺寸列表，用于目标检测
anchor = (0.893, 1.463, 0.245, 0.389, 1.55, 2.58, 0.375, 0.594, 3.099, 5.038, 0.057, 0.090, 0.567, 0.904, 0.101, 0.160, 0.159, 0.255)


#创建 KPU 模型对象
kpu = KPU()

#加载 YOLO 目标检测模型文件（.kmodel 格式）
kpu.load_kmodel("/sd/KPU/yolo_face_detect/yolo_face_detect.kmodel")

#使用 init_yolo2 初始化 YOLO 模型参数
#anchor: 锚框的尺寸列表，用于目标检测
#anchor_num: 锚框的数量
#img_w, img_h: 输入图像的宽度和高度
#net_w, net_h: 模型输入的宽度和高度
#layer_w, layer_h: 模型最终层的宽度和高度
#threshold: 检测目标的置信度阈值
#nms_value: 非最大抑制的 IOU 阈值
#classes: 目标类别数量
kpu.init_yolo2(anchor, anchor_num=9, img_w=320, img_h=240, net_w=320, net_h=256, layer_w=10, layer_h=8, threshold=0.75, nms_value=0.3, classes=1)

try:
    #loop
    while True:
        #par1：主机发送的寄存器地址
        #par2：需要获取的字节数 最大为100
        ret = i2c.get_reg_value(50, 6)
        detect = False
        clock.tick()  #计算每秒帧率
        img = sensor.snapshot()  #从相机获取图像
        #将图像数据复制到 img_obj 对象中，以便传递给 KPU 运行
        img_obj.draw_image(img, 0, 0)
        img_obj.pix_to_ai()
        #使用 KPU 运行目标检测模型
        kpu.run_with_output(img_obj)
        #获取检测结果
        dect = kpu.regionlayer_yolo2()
        #计算帧率
        fps = clock.fps()
        #如果检测到目标
        if len(dect) > 0:
            for l in dect:
                #在图像上绘制检测到的目标框
                img.draw_rectangle(l[0], l[1], l[2], l[3], color=(0, 0, 255))
                face_data[0] = 1                        #face id
                face_data[1] = l[0] & 0xFF              #识别到的人脸中心x轴坐标点低八位
                face_data[2] = (l[0] >> 8) & 0xFF       #识别到的人脸中心x轴坐标点高八位
                face_data[3] = l[1] & 0xFF              #识别到的人脸中心y轴坐标点低八位
                face_data[4] = (l[1] >> 8) & 0xFF       #识别到的人脸中心y轴坐标点高八位
                face_data[5] = l[2] & 0xFF              #识别到的人脸检测框宽度低八位
                face_data[6] = (l[2] >> 8) & 0xFF       #识别到的人脸检测框宽度高八位
                face_data[7] = l[3] & 0xFF              #识别到的人脸检测框高度低八位
                face_data[8] = (l[3] >> 8) & 0xFF       #识别到的人脸检测框高度高八位
                detect = True

        if detect == False:
            for i in range(len(face_data)):
                face_data[i] = 0
        i2c.set_reg_value(0x10, face_data)
        print(" ".join(str(x) for x in face_data))
        #在图像上显示帧率
        img.draw_string(0, 0, "%2.1ffps" % (fps), color=(100, 150, 255), scale=2.0)
        #在 LCD 上显示处理后的图像
        lcd.display(img)

#捕获错误并处理
except Exception as e:
    raise e
finally:
    #若出现错误，则释放 KPU 资源
    kpu.deinit()








