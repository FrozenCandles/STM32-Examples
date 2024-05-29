
import socket

server_name = '192.168.10.1'
server_port = 8266

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((server_name, server_port))

client.send(b'Hello, STM32&ESP8266, I\'m Python from PC')

client.close()

