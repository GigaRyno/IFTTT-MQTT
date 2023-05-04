import time
import paho.mqtt.client as mqtt
import datetime
import json

from ifttt_webhook import IftttWebhook


IFTTT_KEY = "b1aDErR2Xskh_5IaBRh9ge"
ifttt = IftttWebhook(IFTTT_KEY)

id = 'ryan'

client_telemetry_topic = 'champ_iot/ryan'
server_command_topic = client_telemetry_topic
client_name = id + '_info'

mqtt_client = mqtt.Client(client_name)
mqtt_client.connect('broker.hivemq.com')

mqtt_client.loop_start()

def handle_data(message):
    payload = message.payload.decode()
    recTime = datetime.datetime.now()

    jsonData = make_json(payload)

    print("Message received at: ", recTime)
    print("Message data: " + str(jsonData))
    write_to_file(jsonData, recTime)

    if jsonData["Temp"] > 15:
        ifttt.trigger("temp_over_15", value1 ="temp_over_15")

    if jsonData["Humid"] > 10:
        ifttt.trigger("humid_over_10", value1 ="humid_over_10")

def write_to_file(data, timeRecived):
    file = open("temperature.txt", "a")
    file.write(str(timeRecived) + "," + str(data["Temp"]) + "," + str(data["Humid"]) +"\n")
    file.close()

def make_json(dataToDecode):
    return json.loads(dataToDecode)

mqtt_client.subscribe(client_telemetry_topic)
mqtt_client.on_message = handle_data

while True:
    time.sleep(1)