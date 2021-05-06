import os
import time

# 处理中文
import locale
locale.setlocale(locale.LC_CTYPE, 'chinese')

if __name__ == '__main__':
    push_msg = input("请输入更新信息：")
    time_str = time.strftime('%Y年%m月%d日 %H:%M:%S',
                             time.localtime(time.time()))
    os.system('git add .')
    os.system('git commit -m "{:s} - {:s}"'.format(time_str, push_msg))
    os.system('pause')
