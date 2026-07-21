import hiwonder
import sensor
import image
import time
import lcd

i2c = hiwonder.hw_slavei2c()

lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 100)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
clock = time.clock()

color_data = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])

# 输出:
# [[0, 0, 0, 0],
#  [0, 0, 0, 0],
#  [0, 0, 0, 0]]

# 储存6种颜色的LAB阈值
thresholds = [(21, 82, 35, 127, 0, 82),     # Red      index is 0 so code == (1 << 0)
              (33, 100, -46, -26, -61, 127),# Green    index is 1 so code == (1 << 1)
              (34, 100, -20, 18, -41, -11), # Blue     index is 2 so code == (1 << 2)
              (65, 78, -10, -5, 38, 50),    # Yellow   index is 3 so code == (1 << 3)
              (20, 50, 17, 37, -34, -14)]   # Purple   index is 4 so code == (1 << 4)

while True:
    #从传感器捕获一张图像
    img = sensor.snapshot()

    # 初始化每种颜色是否检测到的标志
    detect = False

    #遍历多种颜色
    for blob in img.find_blobs(thresholds, pixels_threshold=100, area_threshold=100, merge=True, margin=10):
        if blob:
            #画方框
            if blob.code() == 1:    #Red
                detect = True
                img.draw_rectangle(blob.rect(), color=(255, 0, 0),thickness = 3)
                img.draw_cross(blob.cx(), blob.cy(), color=(255, 0, 0))
                color_data[0] = 1                            #Color id
                color_data[1] = blob.cx() & 0xFF             #识别到的颜色中心x轴坐标点低八位
                color_data[2] = (blob.cx() >> 8) & 0xFF      #识别到的颜色中心x轴坐标点高八位
                color_data[3] = blob.cy() & 0xFF             #识别到的颜色中心y轴坐标点低八位
                color_data[4] = (blob.cy() >> 8) & 0xFF      #识别到的颜色中心y轴坐标点高八位
                color_data[5] = blob.w() & 0xFF              #识别到的颜色检测框宽度低八位
                color_data[6] = (blob.w() >> 8) & 0xFF       #识别到的颜色检测框宽度高八位
                color_data[7] = blob.h() & 0xFF              #识别到的颜色检测框高度低八位
                color_data[8] = (blob.h() >> 8) & 0xFF       #识别到的颜色检测框高度高八位

            if blob.code() == 2:    #Green
                detect = True
                img.draw_rectangle(blob.rect(), color=(0, 255, 0),thickness = 3)
                img.draw_cross(blob.cx(), blob.cy(), color=(0, 255, 0))
                color_data[0] = 2                            #Color id
                color_data[1] = blob.cx() & 0xFF             #识别到的颜色中心x轴坐标点低八位
                color_data[2] = (blob.cx() >> 8) & 0xFF      #识别到的颜色中心x轴坐标点高八位
                color_data[3] = blob.cy() & 0xFF             #识别到的颜色中心y轴坐标点低八位
                color_data[4] = (blob.cy() >> 8) & 0xFF      #识别到的颜色中心y轴坐标点高八位
                color_data[5] = blob.w() & 0xFF              #识别到的颜色检测框宽度低八位
                color_data[6] = (blob.w() >> 8) & 0xFF       #识别到的颜色检测框宽度高八位
                color_data[7] = blob.h() & 0xFF              #识别到的颜色检测框高度低八位
                color_data[8] = (blob.h() >> 8) & 0xFF       #识别到的颜色检测框高度高八位

            if blob.code() == 4:    #Blue
                detect = True
                img.draw_rectangle(blob.rect(), color=(0, 0, 255),thickness = 3)
                img.draw_cross(blob.cx(), blob.cy(), color=(0, 0, 255))
                color_data[0] = 3                            #Color id
                color_data[1] = blob.cx() & 0xFF             #识别到的颜色中心x轴坐标点低八位
                color_data[2] = (blob.cx() >> 8) & 0xFF      #识别到的颜色中心x轴坐标点高八位
                color_data[3] = blob.cy() & 0xFF             #识别到的颜色中心y轴坐标点低八位
                color_data[4] = (blob.cy() >> 8) & 0xFF      #识别到的颜色中心y轴坐标点高八位
                color_data[5] = blob.w() & 0xFF              #识别到的颜色检测框宽度低八位
                color_data[6] = (blob.w() >> 8) & 0xFF       #识别到的颜色检测框宽度高八位
                color_data[7] = blob.h() & 0xFF              #识别到的颜色检测框高度低八位
                color_data[8] = (blob.h() >> 8) & 0xFF       #识别到的颜色检测框高度高八位

            if blob.code() == 8:    #Yellow
                detect = True
                img.draw_rectangle(blob.rect(), color=(255, 255, 0),thickness = 3)
                img.draw_cross(blob.cx(), blob.cy(), color=(0, 0, 255))
                color_data[0] = 4                            #Color id
                color_data[1] = blob.cx() & 0xFF             #识别到的颜色中心x轴坐标点低八位
                color_data[2] = (blob.cx() >> 8) & 0xFF      #识别到的颜色中心x轴坐标点高八位
                color_data[3] = blob.cy() & 0xFF             #识别到的颜色中心y轴坐标点低八位
                color_data[4] = (blob.cy() >> 8) & 0xFF      #识别到的颜色中心y轴坐标点高八位
                color_data[5] = blob.w() & 0xFF              #识别到的颜色检测框宽度低八位
                color_data[6] = (blob.w() >> 8) & 0xFF       #识别到的颜色检测框宽度高八位
                color_data[7] = blob.h() & 0xFF              #识别到的颜色检测框高度低八位
                color_data[8] = (blob.h() >> 8) & 0xFF       #识别到的颜色检测框高度高八位

            if blob.code() == 16:    #Purple
                detect = True
                img.draw_rectangle(blob.rect(), color=(128, 0, 128),thickness = 3)
                img.draw_cross(blob.cx(), blob.cy(), color=(0, 0, 255))
                color_data[0] = 5                            #Color id
                color_data[1] = blob.cx() & 0xFF             #识别到的颜色中心x轴坐标点低八位
                color_data[2] = (blob.cx() >> 8) & 0xFF      #识别到的颜色中心x轴坐标点高八位
                color_data[3] = blob.cy() & 0xFF             #识别到的颜色中心y轴坐标点低八位
                color_data[4] = (blob.cy() >> 8) & 0xFF      #识别到的颜色中心y轴坐标点高八位
                color_data[5] = blob.w() & 0xFF              #识别到的颜色检测框宽度低八位
                color_data[6] = (blob.w() >> 8) & 0xFF       #识别到的颜色检测框宽度高八位
                color_data[7] = blob.h() & 0xFF              #识别到的颜色检测框高度低八位
                color_data[8] = (blob.h() >> 8) & 0xFF       #识别到的颜色检测框高度高八位

    if detect == False:
        for i in range(len(color_data)):
            color_data[i] = 0
    i2c.set_reg_value(0x00, color_data)
    print(" ".join(str(x) for x in color_data))
    #显示在LCD上
    lcd.display(img)
    #打印帧率

