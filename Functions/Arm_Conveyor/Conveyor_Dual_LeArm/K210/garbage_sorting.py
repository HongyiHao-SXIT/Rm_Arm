import hiwonder
import sensor
import image
import cmath
import time
import lcd
import gc

from maix import KPU

i2c = hiwonder.hw_slavei2c()

lcd.init()                          # Init lcd display
lcd.clear(lcd.RED)                  # Clear lcd screen.

# sensor.reset(dual_buff=True)      # improve fps
sensor.reset()                      # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565) # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)   # Set frame size to QVGA (320x240)
sensor.set_vflip(False)              # 翻转摄像头
sensor.set_hmirror(False)            # 镜像摄像头
sensor.skip_frames(time = 1000)     # Wait for settings take effect.
clock = time.clock()                # Create a clock object to track the FPS.

result = bytearray([0, 0, 0, 0, 0, 0, 0, 0, 0])
print("ready load model")

labels = ["oral_bottle",
          "color_pen",
          "battery",
          "brush",
          "plastic_bottle",
          "umbrella",
          "banana_peel",
          "ketchup",
          "bone",
          "chopstick",
          "plate",
          "cigarette_end"] #类名称，按照label.txt顺序填写

class GarbageCategory:
    RECYCLE = "Recyclable"
    HAZARDOUS = "Hazardous"
    ORGANIC = "Organic"
    OTHERS = "Others"
    UNKNOWN = "Unknown"

# 定义哈希表
garbage_classification = {
    "oral_bottle": GarbageCategory.RECYCLE,
    "color_pen": GarbageCategory.OTHERS,
    "battery": GarbageCategory.HAZARDOUS,
    "brush": GarbageCategory.OTHERS,
    "plastic_bottle": GarbageCategory.RECYCLE,
    "umbrella": GarbageCategory.OTHERS,
    "banana_peel": GarbageCategory.ORGANIC,
    "ketchup": GarbageCategory.ORGANIC,
    "bone": GarbageCategory.ORGANIC,
    "chopstick": GarbageCategory.OTHERS,
    "plate": GarbageCategory.OTHERS,
    "cigarette_end": GarbageCategory.OTHERS
}

anchor = (1.38, 1.41, 2.00, 2.03, 2.78,
            2.81, 3.72, 3.66, 5.66, 5.47) # anchors,使用anchor.txt中第二行的值


kpu = KPU()
# 从sd或flash加载模型
kpu.load_kmodel('/sd/KPU/garbage_sorting/garbage_sorting.kmodel')
kpu.init_yolo2(anchor, anchor_num=(int)(len(anchor)/2), img_w=320, img_h=240, net_w=320 , net_h=240 ,layer_w=10 ,layer_h=8, threshold=0.6, nms_value=0.3, classes=len(labels))


while(True):
    detect = False
    gc.collect()

    clock.tick()
    img = sensor.snapshot()

    kpu.run_with_output(img)
    dect = kpu.regionlayer_yolo2()

    fps = clock.fps()

    if len(dect) > 0:
        for l in dect :
            detect = True
            a = img.draw_rectangle(l[0],l[1],l[2],l[3],color=(0,255,0))
            info = "%s %.3f" % (labels[l[4]], l[5])
            a = img.draw_string(l[0],l[1],info,color=(255,0,0),scale=2.0)
            classification = garbage_classification.get(labels[l[4]], GarbageCategory.UNKNOWN)
            if classification == GarbageCategory.RECYCLE:
                result[0] = 1
                result[1] = (l[0] + (l[2] // 2)) & 0xFF             #识别到的物体中心x轴坐标点低八位
                result[2] = ((l[0] + (l[2] // 2)) >> 8) & 0xFF      #识别到的物体中心x轴坐标点高八位
                result[3] = (l[1] + (l[3] // 2)) & 0xFF             #识别到的物体中心y轴坐标点低八位
                result[4] = ((l[1] + (l[3] // 2)) >> 8) & 0xFF      #识别到的物体中心y轴坐标点高八位
                result[5] = l[2] & 0xFF                          #识别到的物体检测框宽度低八位
                result[6] = (l[2] >> 8) & 0xFF                   #识别到的物体检测框宽度高八位
                result[7] = l[3] & 0xFF                          #识别到的物体检测框高度低八位
                result[8] = (l[3] >> 8) & 0xFF                   #识别到的物体检测框高度高八位
            elif classification == GarbageCategory.HAZARDOUS:
                result[0] = 2
                result[1] = (l[0] + (l[2] // 2)) & 0xFF
                result[2] = ((l[0] + (l[2] // 2)) >> 8) & 0xFF
                result[3] = (l[1] + (l[3] // 2)) & 0xFF
                result[4] = ((l[1] + (l[3] // 2)) >> 8) & 0xFF
                result[5] = l[2] & 0xFF
                result[6] = (l[2] >> 8) & 0xFF
                result[7] = l[3] & 0xFF
                result[8] = (l[3] >> 8) & 0xFF
            elif classification == GarbageCategory.ORGANIC:
                result[0] = 3
                result[1] = (l[0] + (l[2] // 2)) & 0xFF
                result[2] = ((l[0] + (l[2] // 2)) >> 8) & 0xFF
                result[3] = (l[1] + (l[3] // 2)) & 0xFF
                result[4] = ((l[1] + (l[3] // 2)) >> 8) & 0xFF
                result[5] = l[2] & 0xFF
                result[6] = (l[2] >> 8) & 0xFF
                result[7] = l[3] & 0xFF
                result[8] = (l[3] >> 8) & 0xFF
            elif classification == GarbageCategory.OTHERS:
                result[0] = 4
                result[1] = (l[0] + (l[2] // 2)) & 0xFF
                result[2] = ((l[0] + (l[2] // 2)) >> 8) & 0xFF
                result[3] = (l[1] + (l[3] // 2)) & 0xFF
                result[4] = ((l[1] + (l[3] // 2)) >> 8) & 0xFF
                result[5] = l[2] & 0xFF
                result[6] = (l[2] >> 8) & 0xFF
                result[7] = l[3] & 0xFF
                result[8] = (l[3] >> 8) & 0xFF

    if detect == False:
        for i in range(len(result)):
            result[i] = 0
    #print(result)
    i2c.set_reg_value(0x30, result)
    a = img.draw_string(0, 0, "%2.1ffps" %(fps),color=(0,60,255),scale=2.0)
    lcd.display(img)
