import ssl
from kivy.clock import Clock
from kivymd.uix.dialog import MDDialog
from kivymd.uix.button import MDFlatButton
from kivymd.uix.boxlayout import MDBoxLayout
from kivy.uix.image import Image
from kivymd.app import MDApp
from kivy.uix.screenmanager import Screen, ScreenManager
from kivy.storage.jsonstore import JsonStore
from kivy.properties import ListProperty, ObjectProperty, StringProperty
import paho.mqtt.client as mqtt
from typing import Union
from queue import Queue
from collections import deque
from kivy.utils import platform
import kivy
kivy.require('2.1.0')


if platform == 'android':
    from android.permissions import request_permissions, Permission
    request_permissions([
        Permission.INTERNET,
        Permission.READ_EXTERNAL_STORAGE,
        Permission.WRITE_EXTERNAL_STORAGE
    ])


ssl._create_default_https_context = ssl._create_unverified_context

queue_msg = Queue()
queue_log = deque(maxlen=50)
incoming_message = ''
user = ''
password = ''
selected_topic = ''
lamp_power_state = False
lamp_state = False
connection_state = False
topic_list = []
icon_color = ''
icon_color_select = ''

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::SCREEN::::


class MainWindow(Screen):

    scr_manager = ObjectProperty(None)

    def change_screen(self, screen, *args):
        self.ids.scr_manager.current = screen


class HomeScreen(Screen):
    devices_list_id = ObjectProperty(None)

    def on_touch_move(self, touch):
        if touch.x > (touch.ox + 100):
            MDApp.get_running_app().change_screen("datascreen", 'right')


class DataScreen(Screen):
    def on_touch_move(self, touch):
        if touch.x < (touch.ox - 100):
            MDApp.get_running_app().change_screen("homescreen", 'left')


class MenuScreen(Screen):
    def on_touch_move(self, touch):
        if touch.x > (touch.ox + 100):
            MDApp.get_running_app().change_screen("homescreen", 'right')


class InfoScreen(Screen):
    def on_touch_move(self, touch):
        if touch.x > (touch.ox + 100):
            MDApp.get_running_app().change_screen("homescreen", 'right')


class ColorScreen(Screen):
    def on_touch_move(self, touch):
        if touch.x > (touch.ox + 100):
            MDApp.get_running_app().change_screen("homescreen", 'right')


#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::SWITCH::::


class LampIcon(Image):
    def on_touch_down(self, touch):
        if self.collide_point(*touch.pos):
            global lamp_state
            if (connection_state == True):
                if (lamp_power_state == True):

                    if lamp_state == False:
                        lamp_state = True
                        MqttApp.pub(self, 'on')
                    else:
                        lamp_state = False
                        MqttApp.pub(self, 'off')
                else:
                    dialog = MDDialog(
                        text="Device disconnected!",
                        radius=[20, 7, 20, 7],)
                    dialog.open()
            else:
                show_connection_alert()

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::COLOR PICKER::::


class ColorPicker():

    def send_color_picked(self, *args):
        global icon_color_select
        icon_color_select = str(args[0])
        if (connection_state == True):
            if (lamp_power_state == True):
                if (args[0] == 'Red'):
                    MqttApp.pub(self, '255,0,0,255')
                elif (args[0] == 'Pink'):
                    MqttApp.pub(self, '150,0,150,255')
                elif (args[0] == 'Purple'):
                    MqttApp.pub(self, '255,0,150,255')
                elif (args[0] == 'Blue'):
                    MqttApp.pub(self, '0,0,255,255')
                elif (args[0] == 'LightBlue'):
                    MqttApp.pub(self, '20,20,200,255')
                elif (args[0] == 'Cyan'):
                    MqttApp.pub(self, '10,50,255,255')
                elif (args[0] == 'Green'):
                    MqttApp.pub(self, '0,255,0,255')
                elif (args[0] == 'LightGreen'):
                    MqttApp.pub(self, '0,200,0,255')
                elif (args[0] == 'Lime'):
                    MqttApp.pub(self, '50,255,50,255')
                elif (args[0] == 'Yellow'):
                    MqttApp.pub(self, '255,255,0,255')
                elif (args[0] == 'Orange'):
                    MqttApp.pub(self, '255,128,0,255')
                elif (args[0] == 'Violet'):
                    MqttApp.pub(self, '128,0,255,255')
            else:
                dialog = MDDialog(
                    text="Device disconnected!",
                    radius=[20, 7, 20, 7],)
                dialog.open()
        else:
            show_connection_alert()

#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::LAST DEVICE::::

class LastDevice():
    def get_last_device(self):
        global selected_topic
        user_data = JsonStore('user_data.json')
        if user_data.exists('last_device'):
            last_device_data = user_data.get('last_device')
            selected_topic = last_device_data['saved_last_device']
        else:
            selected_topic = ''
            last_device = ''
            user_data.put('last_device', saved_last_device=last_device)

    def put_last_device(self):
        user_data = JsonStore('user_data.json')
        if user_data.exists('last_device'):
            last_device = selected_topic
            user_data.put('last_device', saved_last_device=last_device)
        else:
            last_device = ''
            user_data.put('last_device', saved_last_device=last_device)

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::LOG DIALOG BOX::::


class LogContent(MDBoxLayout):
    log_id = ObjectProperty(None)


class Log_DialogBox():

    dialog = None

    def show_dialog(self, *args):
        self.logs = queue_log
        if not self.dialog:
            self.dialog = MDDialog(
                title='Log:',
                type="custom",
                content_cls=LogContent(),
                buttons=[
                    MDFlatButton(
                        text="CLEAR", on_release=self.clear_log
                    ),
                    MDFlatButton(
                        text="CANCEL", on_release=self.dialog_close
                    ),

                ],
            )

        self.dialog.open()

    def clear_log(self, *args):
        MDApp.get_running_app().clear_queue_log()

    def dialog_close(self, *args):
        self.dialog.dismiss()

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::DIALOG BOX::::


class AddContent(MDBoxLayout):
    pass


class Add_DialogBox():

    dialog = None

    def show_dialog(self, *args):
        if not self.dialog:
            self.dialog = MDDialog(
                type="custom",
                content_cls=AddContent(),
                buttons=[
                    MDFlatButton(
                        text="CANCEL", on_release=self.dialog_close
                    ),
                    MDFlatButton(
                        text="OK", on_release=self.ok
                    ),
                ],
            )
        self.dialog.open()

    def dialog_close(self, *args):
        self.dialog.dismiss()
        self.dialog.content_cls.ids.add_dialog_lbl.text = 'Device name:'

    def ok(self, *args):
        user_data = JsonStore('user_data.json')
        global topic_list

        if user_data.exists('topic_data'):
            topic_data = user_data.get('topic_data')
            topic_list = topic_data['saved_topic_list']

        else:
            topic_list = []
            user_data.put('topic_data', saved_topic_list=topic_list)

        input_topic = self.dialog.content_cls.ids.add_input_topic.text
        if input_topic != '':
            if (input_topic not in topic_list):
                topic_list.append(input_topic)
                user_data.put('topic_data', saved_topic_list=topic_list)
                self.dialog_close(self)
                MDApp.get_running_app().update_spinner_list()
                if selected_topic not in topic_list:
                    MDApp.get_running_app().update_spinner_text()

            else:
                self.dialog.content_cls.ids.add_dialog_lbl.text = 'The device already exists!'


class RemoveContent(MDBoxLayout):
    pass


class Remove_DialogBox():

    dialog = None

    def show_dialog(self, *args):
        if not self.dialog:
            self.dialog = MDDialog(
                type="custom",
                content_cls=RemoveContent(),
                buttons=[
                    MDFlatButton(
                        text="CANCEL", on_release=self.dialog_close
                    ),
                    MDFlatButton(
                        text="OK", on_release=self.ok
                    ),
                ],
            )
        self.dialog.open()

    def dialog_close(self, *args):
        self.dialog.dismiss()
        self.dialog.content_cls.ids.remove_dialog_lbl.text = 'Device name:'

    def ok(self, *args):
        user_data = JsonStore('user_data.json')
        global topic_list
        global selected_topic

        if user_data.exists('topic_data'):
            topic_data = user_data.get('topic_data')
            topic_list = topic_data['saved_topic_list']

        else:
            topic_list = []
            user_data.put('topic_data', saved_topic_list=topic_list)

        input_topic = self.dialog.content_cls.ids.remove_input_topic.text
        if (input_topic not in topic_list):
            self.dialog.content_cls.ids.remove_dialog_lbl.text = 'Device not found!'
        else:
            if input_topic != selected_topic:
                topic_list.remove(input_topic)
                user_data.put('topic_data', saved_topic_list=topic_list)
                self.dialog_close(self)
                MDApp.get_running_app().update_spinner_list()

            else:
                topic_list.remove(input_topic)
                user_data.put('topic_data', saved_topic_list=topic_list)
                self.dialog_close(self)
                MDApp.get_running_app().update_spinner_list()
                MDApp.get_running_app().update_selected_device()
                mqtt = MqttApp()
                mqtt.mqtt_connect()


def show_data_alert():

    dialog = MDDialog(
        text="Fill all fields!",
        radius=[20, 7, 20, 7],)
    dialog.open()


def show_longer_alert():

    dialog = MDDialog(
        text="User and password must be at least 12 characters long",
        radius=[20, 7, 20, 7],)
    dialog.open()


def show_connection_alert():

    dialog = MDDialog(
        text="Client disconnected!",
        radius=[20, 7, 20, 7],)
    dialog.open()

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::MQTT::::


class MqttApp():

    def mqtt_connect(self):

        global client
        client = mqtt.Client(clean_session=True, userdata=True)
        global user
        global password

        try:
            self.mqtt_disconnect()
        except:
            pass

        try:
            user_data = JsonStore('user_data.json')
            profile = user_data.get('profile')
            broker_address = profile['new_host']
            port = 1883
            user = profile['new_user']
            password = profile['new_password']

        except:

            dialog = MDDialog(
                text="Create profile!",
                radius=[20, 7, 20, 7],)
            dialog.open()

        if (selected_topic == '' or selected_topic == '...' or selected_topic == 'No devices'):
            if not topic_list:
                dialog = MDDialog(
                    text="Add device",
                    radius=[20, 7, 20, 7],)
                dialog.open()
            else:
                dialog = MDDialog(
                    text="Select device",
                    radius=[20, 7, 20, 7],)
                dialog.open()
        else:

            if topic_list:
                try:
                    client.on_connect = self.on_connect
                    client.on_disconnect = self.on_disconnect
                    client.on_message = self.on_message
                    client.on_log = self.on_log

                except Exception as e:
                    pass

                try:
                    client.connect(host=broker_address,
                                   port=int(port), keepalive=60)
                    client.loop_start()

                except Exception as e:
                    global connection_state
                    connection_state = False
                    MDApp.get_running_app().update_connection_icon()

    def mqtt_disconnect(self):
        try:
            client.loop_stop()
            client.disconnect()
            global connection_state
            connection_state = False
            MDApp.get_running_app().update_connection_icon()
        except:
            pass

    def pub(self, stato):
        try:
            full_topic = (str(user) + '/' + str(password) +
                          '/wifilamp/' + str(selected_topic))
            client.publish(full_topic, stato)
        except Exception as e:
            pass

    def on_log(self, client, userdata, level, buff):
        queue_log.append(buff)
        MDApp.get_running_app().update_logs()

    def on_connect(self, client, userdata, flags, rc):
        global connection_state
        if rc == 0:
            connection_state = True
            MDApp.get_running_app().update_connection_icon()
            full_topic = (str(user) + '/' + str(password) +
                          '/wifilamp/callback/' + str(selected_topic))
            client.subscribe(topic=full_topic)
        else:
            pass

    def on_disconnect(self, client, userdata, rc):
        global connection_state
        connection_state = False
        MDApp.get_running_app().update_connection_icon()

    def on_message(self, client, userdata, msg):
        queue_msg.put(msg)
        get_msg()

#:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::MAIN::::


class MainApp(MDApp):

    device_connection_icon = StringProperty()
    spinner_text = StringProperty()
    connection_icon = StringProperty()
    user_data = ObjectProperty(None)
    spinner_list = ListProperty()
    logs = StringProperty()
    icon_img = StringProperty()

    add_dialogbox = Add_DialogBox()
    remove_dialogbox = Remove_DialogBox()
    color_picker = ColorPicker()
    log_dialogbox = Log_DialogBox()

    def __init__(self, *args, **kwargs):
        super(MainApp, self).__init__(*args, **kwargs)

        Clock.schedule_interval(self.update_icon_img, 1)
        self.icon_img = 'images/Red.png'
        self.connection_icon = 'earth'
        self.device_connection_icon = 'power-plug-off-outline'
        self.user_data = JsonStore('user_data.json')
        self.spinner_list = topic_list
        self.logs = ''

    def build(self):
        self.theme_cls.primary_palette = "LightBlue"
        self.theme_cls.theme_style = "Light"
        self.theme_cls.primary_hue = "900"
        return MainWindow()

    def on_start(self):
        LastDevice.get_last_device(self)
        self.update_spinner_list()
        self.update_spinner_text()
        self.update_msg_decoder()
        mqtt = MqttApp()
        mqtt.mqtt_connect()

# test resume reconnection.

    def on_resume(self):
        mqtt = MqttApp()
        mqtt.mqtt_connect()

    def on_stop(self):
        mqtt = MqttApp()
        mqtt.mqtt_disconnect()

    def update_icon_img(self, *args):
        global icon_color
        if (icon_color != ''):
            self.icon_img = 'images/' + icon_color + '.png'

    def create_profile(self, host, user, password):

        if(len(user) < 12 or len(password) < 12):
            show_longer_alert()
        else:
            if (host and user and password != ''):
                self.user_data.put('profile',
                                   new_host=host,
                                   new_user=user,
                                   new_password=password,
                                   )
                self.change_screen("homescreen", 'left')
            else:
                show_data_alert()

        global lamp_power_state
        lamp_power_state = False
        self.update_msg_decoder()
        mqtt = MqttApp()
        mqtt.mqtt_connect()

    def connect_button(self):
        try:
            if connection_state == False:
                mqtt = MqttApp()
                mqtt.mqtt_connect()
                update_connection_icon()
        except:
            pass

    def change_screen(self, screen_name, transition):
        screen_manager = self.root.ids["scr_manager"]
        screen_manager.transition.direction = transition
        screen_manager.current = screen_name

    def close_spinner(self):
        if not topic_list:
            dialog = MDDialog(
                text="Add device",
                radius=[20, 7, 20, 7],)
            dialog.open()

    def update_spinner_list(self):
        global topic_list
        user_data = JsonStore('user_data.json')
        if user_data.exists('topic_data'):
            topic_data = user_data.get('topic_data')
            topic_list = topic_data['saved_topic_list']

        else:
            topic_list = []
            user_data.put('topic_data', saved_topic_list=topic_list)

        if topic_list:
            self.spinner_list = topic_list
        else:
            self.spinner_text = 'No devices'
            self.spinner_list = topic_list
            global selected_topic
            selected_topic = ''

    def update_spinner_text(self):
        if selected_topic in topic_list:
            self.spinner_text = selected_topic
        else:
            if topic_list:
                self.spinner_text = '...'
            else:
                self.spinner_text = 'No devices'

    def update_selected_device(self, *args):
        global selected_topic
        global incoming_message
        global lamp_power_state
        global lamp_state
        self.icon_img = 'images/Off.png'
        lamp_state = False
        lamp_power_state = False
        incoming_message = ''

        if len(args) > 0:
            selected_topic = args[1]
        else:
            selected_topic = ''

        if (selected_topic != '...' or selected_topic != 'No devices' or selected_topic != ''):

            if selected_topic in topic_list:
                LastDevice.put_last_device(self)
                mqtt = MqttApp()
                mqtt.mqtt_connect()

        if selected_topic == '':
            LastDevice.put_last_device(self)
        self.update_spinner_text()
        self.update_msg_decoder()

    def rainbow(self):
        if connection_state:
            MqttApp.pub(self, "rainbow")
        else:
            show_connection_alert()

    def update_logs(self):
        self.logs = str(queue_log)

    def clear_queue_log(self):
        queue_log.clear()
        self.logs = ''

    def update_msg_decoder(self):
        global lamp_state
        global lamp_power_state
        global incoming_message
        global icon_color
        global icon_color_select

        if incoming_message == 'off':
            incoming_message = ''
            lamp_power_state = True
            lamp_state = False
            icon_color = 'Off'
        elif incoming_message == 'on':
            incoming_message = ''
            lamp_power_state = True
            lamp_state = True
            icon_color = 'Yellow'
        elif incoming_message == 'rainbow':
            incoming_message = ''
            lamp_power_state = True
            lamp_state = True
            icon_color = 'Yellow'
        elif incoming_message == 'ok':
            incoming_message = ''
            lamp_power_state = True
            lamp_state = True
            icon_color = icon_color_select
        elif incoming_message == 'poff':
            incoming_message = ''
            lamp_power_state = False
            icon_color = 'Off'
            self.device_connection_icon = 'power-plug-off-outline'
        elif incoming_message == 'pon':
            incoming_message = ''
            lamp_power_state = True
        incoming_message = ''
        if lamp_power_state == True:
            self.device_connection_icon = 'power-plug-outline'
        else:
            lamp_state = False
            icon_color = 'Off'
            self.device_connection_icon = 'power-plug-off-outline'

    def update_connection_icon(self):
        if connection_state:
            self.connection_icon = 'earth'
        else:
            self.connection_icon = 'earth-off'
            self.device_connection_icon = 'power-plug-off-outline'


def get_msg():
    while not queue_msg.empty():
        msg = queue_msg.get()
        if msg is None:
            continue
        global incoming_message
        incoming_message = str(msg.payload.decode("utf-8"))
        MDApp.get_running_app().update_msg_decoder()


if __name__ == '__main__':
    MainApp().run()
