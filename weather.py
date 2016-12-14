import requests
url = "http://api.openweathermap.org/data/2.5/weather"
querystring = {"zip":"02215,us","appid":"11dbf4b98c1530853a5ff0671245754f"}
headers = {'cache-control': "no-cache",'postman-token': "811bebb9-acce-df5e-0cc9-dc6aef2e7772"}
response = requests.request("GET", url, headers=headers, params=querystring)
json_obj = response.json()
temp = float(json_obj['main']['temp']) - 273.15
print(temp)
