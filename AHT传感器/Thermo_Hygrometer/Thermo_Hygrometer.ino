//淘宝『有名称的店铺』https://somebodys.taobao.com/
//更新日期 2021/05/04
//Wall Drawing Machine 墙画机 程序
//本程序对应商品 

//web版程序连接：
//Github版链接：  https://github.com/shihaipeng03/Walldraw

//部分系统有不不能正常安装驱动的情况，可以手工安装驱动（CH340 CH341都可以适用）。
//WIN驱动下载链接： https://sparks.gogo.co.nz/assets/_site_/downloads/CH34x_Install_Windows_v3_4.zip  
//其他系统请到  https://sparks.gogo.co.nz/ch340.html  （CH34X驱动大全或自行搜索） 
//注意，x64的win7系统或者是ghost的系统可能会无法安装驱动。


//墙画机无舵机测试程序。本程序只测试2只步进电机，画曲线花纹，修改参数可以改变花纹尺寸和样式。



#include <TinyStepper_28BYJ_48.h>
#include <Wire.h>
#include <SparkFun_Qwiic_Humidity_AHT20.h>
//如果报错缺少此库文件，操作如下
//在arduino IDE的菜单中选择 项目->加载库->管理库（热键 按Ctrl+Shift+I打开） TinyStepper_28BYJ_48 或 SparkFun_Qwiic_Humidity_AHT20，并安装，重新编译即可
//上面报错，请观看视频教程 2分30秒起 https://www.bilibili.com/video/BV1ff4y1975o

#define temp_init 15  //开机前温度转盘的位置  将转盘手工悬挂在15摄氏度的位置
#define humi_init 50  //开机前湿度转盘的位置

//接线方法 
//温度电机的接法
#define tempmotorPin1  4     //  28BYJ48 pin 1 接 4#
#define tempmotorPin2  5     //  28BYJ48 pin 2 接 5#
#define tempmotorPin3  6     //  28BYJ48 pin 3 接 6#  
#define tempmotorPin4  7     //  28BYJ48 pin 4 接 7#

//湿度电机的接法
#define humimotorPin1  8     //  28BYJ48 pin 1 接 8#
#define humimotorPin2  9     //  28BYJ48 pin 2 接 9#
#define humimotorPin3  10     // 28BYJ48 pin 3 接 10#
#define humimotorPin4  11    //  28BYJ48 pin 4 接 11#



TinyStepper_28BYJ_48 temp;
TinyStepper_28BYJ_48 humi;

AHT20 AHT;  //初始化温湿度模块

int temp_calibration=temp_init;
int humi_calibration=humi_init;  //



void setup() 
{  
  Serial.begin(115200);

  temp.connectToPins(tempmotorPin1, tempmotorPin2, tempmotorPin3, tempmotorPin4);  //温度电机 上 外大圈
  humi.connectToPins(humimotorPin1, humimotorPin2, humimotorPin3, humimotorPin4);  //湿度电机 下 内小圈
  temp.setSpeedInStepsPerSecond(256);
  temp.setAccelerationInStepsPerSecondPerSecond(512);
  humi.setSpeedInStepsPerSecond(256);
  humi.setAccelerationInStepsPerSecondPerSecond(512); //初始化步进电机设置速度，加速度等参数

  Wire.begin(); //连接IIC总线

  //检测AHT传感器模块
  if (AHT.begin() == false)
  {
      Serial.println("AHT20/10 not detected. Please check wiring...");
      //温湿度模块读数出现错误
      //将两个转盘都旋转半周，将转盘停留在无数值的位置
      temp.setSpeedInStepsPerSecond(600);
      temp.setAccelerationInStepsPerSecondPerSecond(500);
      temp.setupRelativeMoveInSteps(8192);
      humi.setSpeedInStepsPerSecond(300);
      humi.setAccelerationInStepsPerSecondPerSecond(500);
      humi.setupRelativeMoveInSteps(-4096);     
      while((!temp.motionComplete()) || (!humi.motionComplete()))
          {
            temp.processMovement();
            humi.processMovement();
          }

    //进入死循环，停机
    Serial.println("Shutdown."); 
    while(1);
  }
  //温湿度读数正常
  Serial.println("Test OK!"); 
  delay(1000);
}

void loop()  
{

 
  int n_humi= AHT.getHumidity();
  int n_temp= AHT.getTemperature();  //读取当前湿度 和 温度
  
  
  //外圈温度齿轮192齿，驱动齿轮24齿，步进电机2048个信号转一周  16384信号温度表盘转360度
  //温度表盘刻度每5.625度是1℃  电机需要256个信号，表盘移动1℃。
  temp.moveRelativeInSteps((n_temp - temp_calibration)*256); //计算表盘位置值和当前温度值的“温差” *256信号，旋转到当前温度的位置。
  
  //内圈湿度齿轮96齿，驱动齿轮24T，步进电机2048个信号转一周  8192信号温度表盘转360度
  //湿度表盘刻度每1%湿度是2.64°  0~100共264°  电机需要60个信号，表盘移动1湿度 
  humi.moveRelativeInSteps((n_humi - humi_calibration)*60);
  
  
  humi_calibration = n_humi;
  temp_calibration = n_temp; //更新当前温度湿度值
  
  Serial.print("Humi:");
  Serial.print(humi_calibration);
  Serial.print("\t Temp:");
  Serial.println(temp_calibration); //串口显示检测到的温湿度数值
  
  delay(1000);  //延时
  
}
