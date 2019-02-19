# In Python, print red text using ANSI escape codes
# Doesn't work well in Windows though
print('\033[31m\u001b[1m' + 'Hello' + '\033[0m') 
print(u"\u001b[1m BOLD \u001b[0m\u001b[4m Underline \u001b[0m\u001b[7m Reversed \u001b[0m")
print(u"\u001b[1m\u001b[31m Red Bold \u001b[0m")
print(u"\u001b[4m\u001b[31m Blue Background Underline \u001b[0m")

#Ansi Escap characers:
ANSI_DICT={'close':'\u001b[0m', 'open':u'', 'underlined':'\u001b[4m', 'red':'\u001b[31m'}


print(ANSI_DICT["open"]+ANSI_DICT["red"]+ANSI_DICT["underlined"]+'Is it?'+ANSI_DICT["close"])