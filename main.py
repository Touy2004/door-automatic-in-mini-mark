from playsound import playsound
import paho.mqtt.client as mqtt

functionMode = 0
topic = "soundtouy"

def on_connect(client, userdata, flags, rc):
    global topic

    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe(topic)

def on_message(client, userdata, message):
    global functionMode
    playload = message.payload.decode()

    if playload == "1":
        functionMode = 1
        print("function mode: on")

    if functionMode == 1 and playload == "2":
        playsound("sound.mp3")
        function = 0
        print("function mode: off")
        client.publish(topic, "0")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Disconnected from MQTT broker")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

broker_address = "broker.hivemq.com"  # Replace with your MQTT broker's address
port = 1883  # Default MQTT port
client.connect(broker_address, port)

client.loop_start()

try:
    while True:
        pass
except KeyboardInterrupt:
    client.disconnect()
    client.loop_stop()



