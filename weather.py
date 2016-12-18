import requests
import serial
#run pip install pyserial
ser = serial.Serial('/dev/cu.usbmodem14111', 460800) #set port and speed
url = "http://api.openweathermap.org/data/2.5/weather"
querystring = {"zip":"02215,us","appid":"11dbf4b98c1530853a5ff0671245754f"}
headers = {'cache-control': "no-cache",'postman-token': "811bebb9-acce-df5e-0cc9-dc6aef2e7772"}
response = requests.request("GET", url, headers=headers, params=querystring)
json_obj = response.json()
temp = float(json_obj['main']['temp']) - 273.15
weather = json_obj['weather'][0]['description']
weather_id = json_obj['weather'][0]['id']
print(weather)
print(weather_id)
print(temp)


if(weather_id >= 200 and weather_id < 600):
    print("rain")
    ser.write('0')
elif(weather_id > 600 and weather_id < 700):
    print("snow")
    ser.write('1')
elif(weather_id > 700 and weather_id < 900):
    print("sunny")
    ser.write('2')
elif(weather_id >=900):
    print("exetreme")
    ser.write('3')



#rain, natual disaster, sunny, snow
