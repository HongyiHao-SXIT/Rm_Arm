# AprilTags Example

import hiwonder
import sensor
import image
import time
import math
import lcd

i2c = hiwonder.hw_slavei2c()
#初始化LCD
lcd.init()
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA) # we run out of memory if the resolution is much bigger...
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

# Note! Unlike find_qrcodes the find_apriltags method does not need lens correction on the image to work.

# What's the difference between tag families? Well, for example, the TAG16H5 family is effectively
# a 4x4 square tag. So, this means it can be seen at a longer distance than a TAG36H11 tag which
# is a 6x6 square tag. However, the lower H value (H5 versus H11) means that the false positve
# rate for the 4x4 tag is much, much, much, higher than the 6x6 tag. So, unless you have a
# reason to use the other tags families just use TAG36H11 which is the default family.

# The AprilTags library outputs the pose information for tags. This is the x/y/z translation and
# x/y/z rotation. The x/y/z rotation is in radians and can be converted to degrees. As for
# translation the units are dimensionless and you must apply a conversion function.

# f_x is the x focal length of the camera. It should be equal to the lens focal length in mm
# divided by the x sensor size in mm times the number of pixels in the image.
# The below values are for the OV7725 camera with a 2.8 mm lens.

# f_y is the y focal length of the camera. It should be equal to the lens focal length in mm
# divided by the y sensor size in mm times the number of pixels in the image.
# The below values are for the OV7725 camera with a 2.8 mm lens.

# c_x is the image x center position in pixels.
# c_y is the image y center position in pixels.

f_x = (2.8 / 3.984) * 160 # find_apriltags defaults to this if not set
f_y = (2.8 / 2.952) * 120 # find_apriltags defaults to this if not set
c_x = 160 * 0.5 # find_apriltags defaults to this if not set (the image.w * 0.5)
c_y = 120 * 0.5 # find_apriltags defaults to this if not set (the image.h * 0.5)

result = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])

#def degrees(radians):
    #return (180 * radians) / math.pi

while(True):
    detect = False
    clock.tick()
    img = sensor.snapshot()
    tag = img.find_apriltags(fx=f_x, fy=f_y, cx=c_x, cy=c_y)

    if tag:
        for code in tag:
            pos = code.rect()
            img.draw_rectangle(code.rect(), color = (255, 0, 0))
            #print_args = (code.id(), pos[0] + (pos[2] // 2), pos[1] + (pos[3] // 2), pos[2], pos[3])
            #print("ID: %d, Cx: %f, Cy %f, W %f, H %f" % print_args)
            if code.id() == 1:
                detect = True
                result[0] = 1
                result[1] = (pos[0] + (pos[2] // 2)) & 0xFF             #识别到的物体中心x轴坐标点低八位
                result[2] = ((pos[0] + (pos[2] // 2)) >> 8) & 0xFF      #识别到的物体中心x轴坐标点高八位
                result[3] = (pos[1] + (pos[3] // 2)) & 0xFF             #识别到的物体中心y轴坐标点低八位
                result[4] = ((pos[1] + (pos[3] // 2)) >> 8) & 0xFF      #识别到的物体中心y轴坐标点高八位
                result[5] = pos[2] & 0xFF                          #识别到的物体检测框宽度低八位
                result[6] = (pos[2] >> 8) & 0xFF                   #识别到的物体检测框宽度高八位
                result[7] = pos[3] & 0xFF                          #识别到的物体检测框高度低八位
                result[8] = (pos[3] >> 8) & 0xFF                   #识别到的物体检测框高度高八位
            elif code.id() == 2:
                detect = True
                result[0] = 2
                result[1] = (pos[0] + (pos[2] // 2)) & 0xFF             #识别到的物体中心x轴坐标点低八位
                result[2] = ((pos[0] + (pos[2] // 2)) >> 8) & 0xFF      #识别到的物体中心x轴坐标点高八位
                result[3] = (pos[1] + (pos[3] // 2)) & 0xFF             #识别到的物体中心y轴坐标点低八位
                result[4] = ((pos[1] + (pos[3] // 2)) >> 8) & 0xFF      #识别到的物体中心y轴坐标点高八位
                result[5] = pos[2] & 0xFF                          #识别到的物体检测框宽度低八位
                result[6] = (pos[2] >> 8) & 0xFF                   #识别到的物体检测框宽度高八位
                result[7] = pos[3] & 0xFF                          #识别到的物体检测框高度低八位
                result[8] = (pos[3] >> 8) & 0xFF                   #识别到的物体检测框高度高八位
            elif code.id() == 3:
                detect = True
                result[0] = 3
                result[1] = (pos[0] + (pos[2] // 2)) & 0xFF             #识别到的物体中心x轴坐标点低八位
                result[2] = ((pos[0] + (pos[2] // 2)) >> 8) & 0xFF      #识别到的物体中心x轴坐标点高八位
                result[3] = (pos[1] + (pos[3] // 2)) & 0xFF             #识别到的物体中心y轴坐标点低八位
                result[4] = ((pos[1] + (pos[3] // 2)) >> 8) & 0xFF      #识别到的物体中心y轴坐标点高八位
                result[5] = pos[2] & 0xFF                          #识别到的物体检测框宽度低八位
                result[6] = (pos[2] >> 8) & 0xFF                   #识别到的物体检测框宽度高八位
                result[7] = pos[3] & 0xFF                          #识别到的物体检测框高度低八位
                result[8] = (pos[3] >> 8) & 0xFF                   #识别到的物体检测框高度高八位
    if detect == False:
        for i in range(len(result)):
            result[i] = 0


    i2c.set_reg_value(0x20, result)
    #在LCD上显示处理后的图像
    lcd.display(img)
