## Sebastian Andres - 25/01/2022
## @ssebas.andres ^ gitHub: https://github.com/SebasAndres
import random
from tkinter import *
import webbrowser
from time import sleep
import pyautogui
INSIDE_WND = 10; MAX_URL = 10
url_videos = list(); GOOGLE_SEARCH = '';
END_VIDEO = ""
with open('config.txt') as f:
    line0 = f.readline()
    INSIDE_WND = int("".join(list(line0)[15:-1]))
    line1 = f.readline()
    MAX_URL = int("".join(list(line1)[12:-1]))
    line2 = f.readline()
    num_videos = int("".join(list(line2)[12:-1]))
    for k in range(num_videos):
        line_k = f.readline()
        url_videos.append("".join(list(line_k)[4:-1]))
    gS = f.readline()
    GOOGLE_SEARCH = "".join(list(gS)[11:-1])
    end_line = f.readline()
    END_VIDEO = "".join(list(end_line)[4:-1])
TITLE = "DONDE T SENTASTE"
webbrowser.open("https://i.ytimg.com/vi/shZ1TyrJn4M/sddefault.jpg")
sleep(2); q = random.randint(0, len(url_videos)-1)
for i in range(MAX_URL):
    webbrowser.open(url_videos[q])
sleep(2)
root = Tk(); root.geometry("500x500")
ICON = PhotoImage(file='images/pedro.png')
MEME = PhotoImage(file='images/pedro.png')
root.title(TITLE)
root.tk.call('wm', 'iconphoto', root._w, ICON)
widget = Label(root, image=MEME).pack()
for u in range(INSIDE_WND-1):
    wnd = Toplevel(); wnd.title(TITLE)
    wnd.geometry("500x500"); wnd.tk.call('wm', 'iconphoto', wnd._w, ICON)
    widget = Label(wnd, image=MEME).pack()
sleep(4)
webbrowser.open("https://translate.google.com/?sl=de&tl=es&"); sleep(1.5)
pyautogui.typewrite("Narr wenn du das liest"); sleep(3)
webbrowser.open("https://google.com"); sleep(2)
pyautogui.typewrite(GOOGLE_SEARCH)
pyautogui.press("enter"); sleep(3);
webbrowser.open(END_VIDEO);
mainloop();