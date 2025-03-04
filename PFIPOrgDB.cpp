#include "pch.h"
#include "framework.h"
//#include "stdafx.h"
//#include <winsock.h>

#include "PathFinder.h"

#include "MainFrm.h"
#include "PathFinderDoc.h"
#include "PathFinderView.h"

//#include "winiphelper.h"
#include "traceroute.h"


struct STATICIPDATA g_staticIPOrgDB[]= {
{"1.6.0.0/15","Sify"},
{"1.22.0.0/15","Tikona Digital Networks"},
{"1.39.0.0/16","Vodafone"},
{"1.186/16","D-VoiS Broadband"},
{"3.0.0.0/8","Amazon"},
{"8.0.0.0/9","Google"},
{"11.0.0.0/8","-"},
{"13.32.0.0/12","Amazon"},
{"13.64.0.0/11","Microsoft"},
{"13.96.0.0/13","Microsoft"},
{"13.104.0.0/14","Microsoft"},
{"13.124.0.0/14","Amazon"},
{"13.224.0.0/14","Amazon"},
{"13.232.0/14","Amazon"},
{"13.248.0.0/14","Amazon"},
{"14.96.0.0/14","TATA Communications"},
{"14.102.0.0/17","WorldPhone"},
{"14.139.0.0/16","NKN"},
{"14.140.0.0/14","TATA Communications"},
{"15.185.0.0/16","Amazon"},
{"15.206.0.0/15","Amazon"},
{"18.128.0.0/9","Amazon"},
{"20.180.0.0/14","Microsoft"},
{"20.184.0.0/13","Microsoft"},
{"23.0.0.0/12","Akamai"},
{"23.1.36/24","Akamai"},
{"23.1.37/24","Akamai"},
{"23.3.70/24","Akamai"},
{"23.8.176/24","Akamai"},
{"23.9.16/24","Akamai"},
{"23.9.18/24","Akamai"},
{"23.9.31/24","Akamai"},
{"23.15.37/24","Akamai"},
{"23.15.110/24","Akamai"},
{"23.20.0.0/14","Amazon"},
{"23.32.0.0/11","Akamai"},
{"23.64.0.0/14","Akamai"},
{"23.72.0.0/13","Akamai"},
{"23.96.0.0/13","Microsoft"},
{"23.192.0.0/11","Akamai"},
{"23.235.192.0/19","InMotion Hosting"},
{"27.48.0.0/15","Ortel"},
{"27.61.0.0/18","Airtel"},
{"27.97/16","Idea"},
{"27.106.120.0/21","Syscon Infoway"},
{"27.109.0.0/19","Blazenet"},
{"27.123.43/24","Yahoo"},
{"27.248.0.0/14","Aircel"},
{"31.13.64.0/18","Facebook "},
{"34.192.0.0/10","Amazon"},
{"35.152.0.0/13","Amazon"},
{"35.160.0.0/12","Amazon"},
{"35.176.0.0/13","Amazon"},
{"35.184.0.0/13","Google"},
{"35.192.0.0/12","Google"},
{"35.208.0.0/12","Google"},
{"35.224.0.0/12","Google"},
{"35.240.0.0/13","Google"},
{"36.255.0.0/22","Mira Consulting"},
{"40.64.0.0/13","Microsoft"},
{"40.74.0.0/15","Microsoft"},
{"40.76.0.0/14","Microsoft"},
{"40.80.0.0/12","Microsoft"},
{"40.96.0.0/12","Microsoft"},
{"40.112.0.0/13","Microsoft"},
{"40.120.0.0/14","Microsoft"},
{"40.124.0.0/16","Microsoft"},
{"40.125.0.0/17","Microsoft"},
{"40.126.0.0/18","Microsoft"},
{"42.104.0.0/17","Vodafone"},
{"42.108.0.0/14","Vodafone"},
{"43.227.132/22","Power Grid"},
{"44.192.0.0/10","Amazon"},
{"45.60.0.0/16","Incapsula Inc"},
{"45.113.122.0/24","Hostgator India"},
{"45.114.244.0/22","CtrlS"},
{"45.123.16.0/22","Reliance Jio"},
{"45.125.116/22","Limras"},
{"45.249.232.0/22","Power Grid"},
{"47.74.0.0/15","Alibaba"},
{"47.76.0.0/14","Alibaba"},
{"47.80.0.0/13","Alibaba"},
{"47.88.0.0/14","Alibaba"},
{"47.250.0.0/15","Alibaba"},
{"47.252.0.0/15","Alibaba"},
{"47.254.0.0/16","Alibaba"},
{"49.32/12","Reliance Jio"},
{"49.50.64.0/18","CYBER FUTURISTICS"},
{"49.143.252.0/22","SMARTLINKLINK BROADBAND"},
{"49.200.0.0/14","TATA Teleservice"},
{"49.205.72.0/22","ACT Broadband"},
{"49.206/16","ACT Broadband"},
{"49.207.48.0/20","ACT Broadband"},
{"49.238.32/18","Southern Online Services"},
{"49.248.248.0/21","TATA Teleservice"},
{"50.56.0.0/17","Rackspace Hosting"},
{"50.56.128.0/20","Rackspace Hosting"},
{"50.56.144.0/22","Rackspace Hosting"},
{"50.112.0.0/16","Amazon"},
{"51.103.0.0/16","Microsoft"},
{"51.104.0.0/15","Microsoft"},
{"51.140.0.0/14","Microsoft"},
{"51.144.0.0/15","Microsot"},
{"52.0.0.0/11","Amazon"},
{"52.32.0.0/11","Amazon"},
{"52.66.43/24","Amazon"},
{"52.66.46/24","Amazon"},
{"52.66.57/24","Amazon"},
{"52.66.77/24","Amazon"},
{"52.66.88/24","Amazon"},
{"52.66.103/24","Amazon"},
{"52.66.115/24","Amazon"},
{"52.66.131/24","Amazon"},
{"52.66.148/24","Amazon"},
{"52.66.151/24","Amazon"},
{"52.66.190/24","Amazon"},
{"52.64.0.0/12","Amazon"},
{"52.84.0.0/14","Amazon"},
{"52.88.0.0/13","Amazon"},
{"52.96.0.0/12","Microsoft"},
{"52.112.0.0/14","Microsoft"},
{"52.132.0.0/14","Microsoft"},
{"52.136.0.0/13","Microsoft"},
{"52.145.0.0/16","Microsoft"},
{"52.146.0.0/15","Microsoft"},
{"52.148.0.0/14","Microsoft"},
{"52.152.0.0/13","Microsoft"},
{"52.160.0.0/11","Microsoft"},
{"52.192.0.0/11","Amazon"},
{"54.192/11","Amazon"},
{"52.224.0.0/11","Microsoft"},
{"54.72.0.0/13","Amazon"},
{"54.80.0.0/12","Amazon"},
{"54.160.0.0/12","Amazon"},
{"54.176.0.0/12","Amazon"},
{"54.224.0.0/12","Amazon"},
{"54.240.0.0/12","Amazon"},
{"58.68.0/17","Dishnet Wireless"},
{"59.88/13","BSNL"},
{"59.96/14","BSNL"},
{"59.144/15","Airtel"},
{"59.160/14","TATA Communications"},
{"59.164/15","TATA Communications"},
{"59.176/13","MTNL"},
{"59.184/15","MTNL"},
{"60.243/16","Hathway"},
{"60.254.0/17","Hathway"},
{"61.0/14","BSNL"},
{"61.8.128/19","NET4INDIA"},
{"61.11.0/17","TATA Communications"},
{"61.12.0/17","Primus"},
{"61.16.128/17","Primus"},
{"61.17/16","TATA Communications"},
{"61.95.128/17","Airtel"},
{"61.246/16","Airtel"},
{"61.247.224/19","Airtel"},
{"63.243.128.0/17","TATA Communications"},
{"64.86.0.0/16","TATA Communications"},
{"64.185.181/24","BitGravity"},
{"64.233.160.0/19","Google"},
{"66.110.0.0/17","TATA Communications"},
{"66.198.0.0/17","TATA Communications"},
{"66.198.128.0/18","TATA Communications"},
{"66.249.64.0/19","Google"},
{"68.80.0.0/13","Comcast Cable Communications"},
{"68.183.0.0/16","DigitalOcean"},
{"72.14.192.0/18","Google"},
{"72.52.0.0/18","Akamai"},
{"72.246.77/24","Akamai"},
{"72.246.79/24","Akamai"},
{"72.246.135/24","Akamai"},
{"74.119.76.0/22","Facebook"},
{"74.125.0.0/16","Google"},
{"75.144.0.0/13","Comcast Cable Communications"},
{"80.231.0.0/16","TATA Communications"},
{"96.6.0.0/15","Akamai"},
{"96.16.0.0/15","Akamai"},
{"98.129.0.0/17","Rackspace Hosting"},
{"98.129.128.0/18","Rackspace Hosting"},
{"98.129.192.0/19","Rackspace Hosting"},
{"98.129.224.0/22","Rackspace Hosting"},
{"98.129.228.0/24","Rackspace Hosting"},
{"99.78.128.0/17","Amazon"},
{"99.79.0.0/16","Amazon"},
{"99.80.0.0/15","Amazon"},
{"99.82.0.0/17","Amazon"},
{"99.82.128.0/18","Amazon"},
{"99.83.64.0/18","Amazon"},
{"99.83.128.0/17","Amazon"},
{"99.84.0.0/16","Amazon"},
{"99.85.128.0/17","Amazon"},
{"99.86.0.0/16","Amazon"},
{"99.87.0.0/17","Amazon"},
{"99.87.128.0/18","Amazon"},
{"100.64.0.0/10","-"},
{"103.1.112.1/24","Poineer Elab"},
{"103.4.252.0/22","FlipKart"},
{"103.7.128.0/22","Power Grid"},
{"103.8.124/22","CtrlS "},
{"103.8.188.0/23","MH Gov"},
{"103.8.248.0/22","AMTRON"},
{"103.13.178.0/23","Sun TV"},
{"103.14.120.0/22","Good Domain Registry"},
{"103.16.68.0/22","GATIK"},
{"103.16.144.0/22","Dream Tech"},
{"103.16.200.0/22","ACT Boradband"},
{"103.19.88.0/22","NxtGen Datacenter"},
{"103.21.58/23","Public Domain Registry"},
{"103.21.124.0/22","IIT Mumbai"},
{"103.23.150.0/24","Maharastra Government"},
{"103.27.8.0/22","IIT Delhi"},
{"103.27.168.0/22","Web Werks"},
{"103.29.44.0/22","Vodafone"},
{"103.37.200.0/23","IIT Roorkee"},
{"103.53.41/24","Hostgator India"},
{"103.59.8.0/22","BSNL"},
{"103.68.220.0/23","SBI"},
{"103.80.250/24","Andhra Bank"},
{"103.94.206.0/24","MP Gov"},
{"103.119.240.0/22","Power Grid"},
{"103.122.128.0/22","TG Gov"},
{"103.138.196.0/22","KA Gov"},
{"103.196.117.0/24","Andhra Bank"},
{"103.203.136.0/22","RJ Gov"},
{"103.218.230/24","Power Grid"},
{"103.225.12.0/22","BSNL"},
{"103.230.84/22","NxtGen"},
{"103.231.8.0/22","CtrlS"},
{"103.231.79/24","CtrlS"},
{"103.233.124/22","CtrlS"},
{"103.234.162.0/24","GJ Gov"},
{"103.241.136.0/22","CtrlS"},
{"103.241.144.0/22","CtrlS"},
{"103.241.180.0/22","CtrlS"},
{"103.247.80/22","Spidigo"},
{"103.248.82/24","CtrlS"},
{"103.251.100/22","Renaissance "},
{"104.16/12","CloudFlare"},
{"104.40.0.0/13","Microsoft"},
{"104.64.0.0/10","Akamai"},
{"104.154.0.0/15","Google"},
{"104.196.0.0/14","Google"},
{"104.208.0.0/13","Microsoft"},
{"104.244.40.0/21","Twitter"},
{"105.16.8.0/21","Seacom"},
{"106.51.0.0/16","ACT Broadband"},
{"106.66.128.0/18","Idea Cellular"},
{"106.76.0.0/14","Idea Cellular"},
{"106.200.12.0/22","Airtel"},
{"106.206/16","Airtel"},
{"107.20.0.0/14","Amazon"},
{"107.154.0.0/16","Incapsula Inc"},
{"107.155.0.0/18","Zenlayer Inc"},
{"108.162.207/24","CloudFlare"},
{"108.170.192.0/18","Google"},
{"108.177.0.0/17","Google"},
{"110.172.24/21","Spidigo"},
{"110.172.128/18","WorldPhone"},
{"110.225/16","Airtel"},
{"110.226/16","Airtel"},
{"110.232.176.0/22","EdgeCast Networks"},
{"110.234.0.0/15","Tulip"},
{"111.91.0/17","Honesty Net Solution"},
{"111.93.0.0/16","TATA Tele Services"},
{"111.118.176.0/20","CYBER FUTURISTICS"},
{"112.79.64.0/18","Vodafone"},
{"112.110.32.0/16","Idea Cellular"},
{"112.133.192/18","RailTel"},
{"112.196.0.0/17","ECL Telecom"},
{"113.30.128.0/17","NET4INDIA"},
{"113.193/16","Tikona Digital Networks"},
{"114.31.224.0/20","Rediff"},
{"114.69.224/19","WorldPhone"},
{"114.79.192.1/18","Dishnet Wireless"},
{"114.143.0.0/16","TATA Communications"},
{"115.96.0.0/14","Hathway"},
{"115.108.0.0/14","TATA Communications"},
{"115.112.0.0/13","TATA Communications"},
{"115.124.104.0/24","ESDS"},
{"115.124.112.0/20","ESDS"},
{"115.184.0.0/15","Reliance"},
{"115.240.0.0/12","Reliance"},
{"116.50.64.0/18","RIL"},
{"116.72.0.0/14","Hathway"},
{"116.119/16","Airtel"},
{"116.193.160/21","CONJOINIX TECHNOLOGIES"},
{"116.202.224.0/19","MTS"},
{"116.203.32.0/19","MTS"},
{"116.203.192.0/19","MTS"},
{"116.214.24/21","Tulip"},
{"117.18.232/24","EdgeCast Networks"},
{"117.55.240/21","CJ ONLINE"},
{"117.96/14","Airtel"},
{"117.192/10","BSNL"},
{"118.67.224/19","NET4INDIA"},
{"118.88.0.0/21","Quatrro BPO Solutions"},
{"118.91.232/21","ABT Info Systems"},
{"118.94.0.0/15","SIFY"},
{"118.102.128.0/17","Dishnet Wireless"},
{"118.185/16","Vodafone"},
{"118.214.0.0/16","Akamai"},
{"118.215.0.0/17","Akamai"},
{"118.215.128.0/18","Akamai"},
{"119.18.57.0/24","Hostgator India "},
{"119.18.58.0/23","Hostgator India "},
{"119.18.60.0/22","Hostgator India "},
{"119.82.64.0/18","SPECTRA NET"},
{"119.226.0.0/15","SIFY"},
{"119.252.144/20","Rediff.com"},
{"120.56.0.0/13","MTNL"},
{"120.88.32.0/20","Honesty Net Solutions"},
{"120.138.8.0/22","CtrlS"},
{"121.101.144.0/20","Yahoo India"},
{"121.240.0.0/13","TATA Communications"},
{"122.15.0.0/16","Vodafone"},
{"122.98.0.0/16","Infosys"},
{"122.160/12","Airtel"},
{"122.176/13","Airtel"},
{"122.184/14","Airtel"},
{"122.248.161.0/24","IBM India"},
{"122.252.136.0/21","Akamai"},
{"122.252.224.0/19","RailTel"},
{"123.63/16","Vodafone"},
{"123.108.32.0/19","NetMagic"},
{"123.108.224/21","Honesty Net Solutions"},
{"123.176.32.0/20","ACT Broadband"},
{"123.201/16","You tele"},
{"123.236/14","Reliance"},
{"124.7/16","SIFY"},
{"124.30/16","SIFY"},
{"124.40.244.0/22","BBNL"},
{"124.123/16","ACT Broadband"},
{"124.124/15","Reliance"},
{"124.153.64/18","NetMagic"},
{"124.247.192/18","TULIP"},
{"125.16/13","Airtel"},
{"125.56.222/24","Akamai"},
{"125.62.128/18","Airtel"},
{"125.63.64/18","SPECTRA NET"},
{"125.99/16","Hathway"},
{"125.252.226/24","Akamai"},
{"125.252.248/21","Akamai"},
{"128.1.0.0/16","Zenlayer Inc"},
{"129.134.0.0/16","Facebook"},
{"130.211.11/24","Google"},
{"134.209.0.0/16","DigitalOcean"},
{"136.232/15","Reliance Jio"},
{"137.97/16","Reliance Jio"},
{"138.197.0.0/16","DigitalOcean"},
{"139.59.0.0/16","DigitalOcean"},
{"140.91.0.0/16","Oracle Corporation"},
{"141.101.124/24","CloudFlare"},
{"142.250.0.0/15","Google"},
{"143.204.0.0/16","Amazon"},
{"144.16/16","ERNET"},
{"148.62.0.0/16","Rackspace Hosting"},
{"149.129.128.0/18","Alibaba"},
{"150.107.120.0/22","TATA Communications"},
{"150.222.0.0/16","Amazon"},
{"151.101.0.0/16","Fastly"},
{"152.195.56/24","ANS"},
{"152.199.39/24","ANS"},
{"154.66.246.0/24","WIOCC"},
{"154.66.247.0/24","WIOCC"},
{"157.32/12","Reliance Jio"},
{"157.49/16","Reliance Jio"},
{"157.227/16","TCS"},
{"157.240.0.0/16","Facebook"},
{"158.144/16","TIFR"},
{"159.65.144/16","DigitalOcean"},
{"159.182.104/24","Pearson Data Center"},
{"159.182.107/24","Pearson Data Center"},
{"161.47.0.0/16","Rackspace Hosting"},
{"162.240.0.0/15","Unified Layer"},
{"162.243.0.0/16","DigitalOcean"},
{"163.47.12.0/22","MTS"},
{"163.53.76.0/22","Flipkart"},
{"163.53.204.0/22","Rainbow communications"},
{"163.122/16","TCS"},
{"163.171.64.0/18","CDNetworks Inc"},
{"163.171.128.0/17","CDNetworks Inc"},
{"164.100/15","GoI"},
{"164.164/16","STPI Bangalore"},
{"167.82.128/24","Fastly"},
{"167.71.0.0/16","DigitalOcean"},
{"169.149.192.0/18","Reliance Jio"},
{"171.48.0.0/19","Airtel"},
{"172.217.0.0/16","Google"},
{"172.253.0.0/16","Google"},
{"173.222.16/20","Akamai"},
{"173.222.64/20","Akamai"},
{"173.223.49/24","Akamai"},
{"173.252.64.0/18","Facebook"},
{"180.87.0.0/17","TATA Communications"},
{"180.87.128.0/18","TATA Communications"},
{"180.92.160.0/20","Trimax IT"},
{"180.149.32.0/21","MTS"},
{"180.149.40.0/21","MTS"},
{"180.149.48.0/20","NKN"},
{"180.149.240.0/22","Web Werks"},
{"180.149.244.0/23","Web Werks"},
{"180.149.246.0/24","Web Werks"},
{"180.149.247.0/25","Web Werks"},
{"180.149.247.128/26","Web Werks"},
{"180.149.247.192/27","Web Werks"},
{"180.149.247.224/28","Web Werks"},
{"180.149.247.240/29","Web Werks"},
{"180.149.247.248/30","Web Werks"},
{"180.149.247.252/31","Web Werks"},
{"180.149.247.254/32","Web Werks"},
{"180.151.0/16","SPECTRA NET"},
{"180.178.0.0/19","Uninor"},
{"180.179.0.0/16","NetMagic"},
{"180.215/16","MTS"},
{"180.222.112.0/21","Yahoo"},
{"180.233.120.0/22","SIKKANET"},
{"182.18.128.0/18"," CtrlS"},
{"182.18.167/24","CtrlS"},
{"182.19.0.0/17","Vodafone"},
{"182.64.0.0/12","Airtel"},
{"183.82.0.0/16","ACT Broadband"},
{"183.83.0.0/16","ACT Broadband"},
{"183.87.0.0/16","Nivyah Broadband"},
{"184.24.0.0/13","Akamai"},
{"184.50.0.0/15","Akamai"},
{"184.84.0.0/14","Akamai"},
{"184.106.56.0/21","Rackspace Hosting"},
{"184.106.64.0/18","Rackspace Hosting"},
{"184.106.128.0/17","Rackspace Hosting"},
{"195.66.224.0/22","London Internet Exchange (LINX)"},
{"196.1.109/24","CDAC"},
{"196.1.113/24","ERNET "},
{"196.1.114/24","ERNET"},
{"196.12.32/19","STPI"},
{"196.15.16/20","Oracle"},
{"198.18.0.0/15","-"},
{"198.32.202/24","NIXI"},
{"198.46.80.0/20","InMotion Hosting"},
{"198.61.128.0/17","Rackspace Hosting"},
{"198.200.130/24","Microsoft"},
{"199.232.20/16","Fastly"},
{"202.1.120/21","OrbiTech Solutions"},
{"202.3.75/24","Cable & Wireless Global"},
{"202.3.76/24","Cable & Wireless Global"},
{"202.3.77/24","IIT Kanpur"},
{"202.3.80/20","BHASKAR BROADBAND"},
{"202.3.112/20","Network Appliance"},
{"202.4.160/19","Vasnet"},
{"202.6.80/24","Hutch"},
{"202.6.99/24","Symphony Services"},
{"202.7.52/22","Tata BP Solar"},
{"202.7.56/22","Tata BP Solar"},
{"202.9.112/20","Astro Network"},
{"202.9.128/18","TATA Communications"},
{"202.9.192/20","Spectramind eServices"},
{"202.12.16/24","Affliated Computer Services"},
{"202.12.243/24","khoday Contact Center"},
{"202.14.69/24","Madgen Solutions LLC"},
{"202.14.72/24","OfficeTiger Database Systems"},
{"202.21.129/24","Hinduja TMT"},
{"202.21.134/23","RevitSystems"},
{"202.21.147/24","Sasken"},
{"202.38.180/22","HP Globalsoft"},
{"202.40.6/24","iEnergizer"},
{"202.41.0.0/23","Institute of Health Systems"},
{"202.41.2.0/23","WNS Global Services"},
{"202.41.4.0/22","ERNET"},
{"202.41.8.0/23","ERNET"},
{"202.41.10.0/24","Jawaharlal Nehru University"},
{"202.41.11.0/24","ERNET"},
{"202.41.12.0/23","ERNET"},
{"202.41.16.0/20","ERNET"},
{"202.41.32.0/19","BHEL"},
{"202.41.64.0/18","ERNET"},
{"202.41.148/22","Greenfield"},
{"202.41.204.0/22","VCUSTOMER"},
{"202.41.224/20","REACH"},
{"202.43.219/24","Yahoo India"},
{"202.44.137/24","IQSNET"},
{"202.45.6.0/23","Symphony Services Corporation"},
{"202.45.10.0/23","Zapapp India"},
{"202.46.19/24","Yahoo India"},
{"202.46.22.0/23","Qualcomm Inc"},
{"202.46.192/20","Cyquator Technologies Limited"},
{"202.46.208/20","I-flex solutions Ltd"},
{"202.47.124.0/24","Magma Design Automation"},
{"202.47.127.0/24","OfficeTiger Database Systems"},
{"202.53.8/21","Vebtel"},
{"202.53.64/19","Nettlinx Limited"},
{"202.53.96/20","PBC Ventures Ltd"},
{"202.54/16","TATA Communications"},
{"202.55.0/19","SAIL"},
{"202.56.96/19","HCL"},
{"202.56.192/18","Airtel"},
{"202.58.132.0/22","WNS Global Services"},
{"202.59.2.0/23","Jubilant Organosys"},
{"202.60.128/19","Rolta India Limited"},
{"202.62.64/19","CityOnline Services Ltd"},
{"202.62.224/20","Ortel Communications"},
{"202.62.252.0/24","Madgen Solutions LLC"},
{"202.63.96/19","Southern Online Services"},
{"202.63.160/19","Exatt Technologies Pvt. Ltd"},
{"202.63.252.0/23","Stream International"},
{"202.65.128/19","Pioneer Online Pvt. Ltd"},
{"202.67.4/22","Mahindra BT"},
{"202.68.128/19","BSES TeleCom Limited"},
{"202.70.192/20","India Online Network"},
{"202.71.128/19","NET4INDIA"},
{"202.72.248.0/22","Gemini Software Solutions"},
{"202.75.192/20","WorldCom Communications India"},
{"202.77.137.0/24","Bertelsmann Marketing Services"},
{"202.78.160/20","SHYAM INTERNET SERVICES"},
{"202.79.248.0/22","HP Globalsoft"},
{"202.80.48.0/20","MCI"},
{"202.81.128/19","Spectramind eServices Pvt Ltd"},
{"202.81.192/20","L&T Netcom Limited"},
{"202.83.16.0/20","ACT Broadband"},
{"202.83.32/19","ASIANET"},
{"202.86.4.0/22","Yahoo India"},
{"202.87.32/19","NetMagic"},
{"202.88.128/18","Hathway"},
{"202.88.208/20","Hathway"},
{"202.88.224/20","ASIANET"},
{"202.88.240/20","ASIANET"},
{"202.89.64/20","World Phone Internet Services"},
{"202.90.96/20","Descon Limited"},
{"202.91.64/19","SwiftMail Communications Limited"},
{"202.91.136/21","iGATE"},
{"202.92.8/21","LEVELUP NETWORK"},
{"202.92.224/20","Airtel"},
{"202.93.154.0/24","IRCTC (Railway)"},
{"202.122.16/21","Kasturi.com"},
{"202.123.32/20","Airtel"},
{"202.125.80/20","Astra Infonets (Pvt) Ltd"},
{"202.129.196.0/22","Wireless Solution India Pvt Ltd"},
{"202.129.240.0/23","EMTICI Engineering"},
{"202.131.96/19","Blazenet Pvt Ltd"},
{"202.131.128/19","Estel Communications"},
{"202.133.48/20","Excell Media"},
{"202.134.144/20","7 STAR Dot Com"},
{"202.134.160/19","7 STAR Dot Com"},
{"202.134.192/20","Hughes Escorts Communications Ltd"},
{"202.137.208/20","Ortel Communications"},
{"202.137.232/21","Rediff.com"},
{"202.138.96/19","Reliance"},
{"202.140.32/19","Estel Communications"},
{"202.140.128/19","Global Tele-Systems Limited"},
{"202.141.0/17","ERNET"},
{"202.141.128/19","ERNET"},
{"202.142.64/18","Zee Telefilms Ltd"},
{"202.144.0/17","SIFY"},
{"202.146.192/20","Spectrum Softtech Solutions Pvt Ltd"},
{"202.148.32/20","Spidernet Software Solution"},
{"202.148.192/20","Dishnet Wireless"},
{"202.149.32/19","Exatt Technologies Pvt. Ltd"},
{"202.149.192/19","TATA Communications"},
{"202.151.128/19","Global Tele-Systems Limited"},
{"202.153.32/20","Excell Media Pvt Ltd"},
{"202.154.160/20","SOFTCELL Technologies Limited"},
{"202.157.64/19","Data infosys"},
{"202.157.190/23","Asianfrontiers Network Services"},
{"202.159.192/18","MTNL"},
{"202.160.160/20","D2V ISP PVT.LTD"},
{"202.162.48/20","HCL Comnet"},
{"202.162.224/19","Netcore Solutions Pvt Ltd"},
{"202.164.32/19","ECL Telecom"},
{"202.164.96/19","Wilnet Communications Pvt Ltd"},
{"202.177.144/18","SIFY"},
{"202.177.224/19","FiveNetwork Solutions"},
{"202.179.64/19","Ankhnet Informations Pvt. Ltd"},
{"202.183.64/20","TATA Power"},
{"202.189.224.0/19","TATA Tele"},
{"202.191.64/19","KELNET"},
{"202.191.128.0/17","SIFY"},
{"203.33.198.0/23","Godrej Infotech"},
{"203.27.235/24","ICICI"},
{"203.62.172/22","Virtusa"},
{"203.76.128/19","HCL INFINET"},
{"203.76.176/20","Broadband Pacenet (India) Pvt Ltd"},
{"203.77.177/24","Birla"},
{"203.77.192/20","Guj Info Petro Ltd"},
{"203.78.128/19","OPTO Networks"},
{"203.78.208.0/20","MCI"},
{"203.84.220/23","Yahoo India"},
{"203.86.96/19","Palcom Online India"},
{"203.88.0/19","Hutch"},
{"203.88.128/19","ICENET"},
{"203.90.64/18","HCL INFINET"},
{"203.91.192/19","WIPRO Technologies"},
{"203.92.32/19","SPECTRA NET"},
{"203.94.192/18","MTNL"},
{"203.99.192/19","Cognizant Technology Solution"},
{"203.100.64.0/20","STPI"},
{"203.101.0/17","Airtel"},
{"203.104.16.0/21","Yahoo India"},
{"203.105.160.0/19","HCL BPO"},
{"203.109.64/18","You tele"},
{"203.110.80/20","delDSL Internet Pvt. Ltd"},
{"203.110.240/21","IIT Kharagpur"},
{"203.112.128/19","Nextgen Communications"},
{"203.115.64/19","Broadband Pacenet (India) Pvt Ltd"},
{"203.115.96/19","PRIMENET GLOBAL LTD"},
{"203.119.49.0/24","NIXI"},
{"203.122.0/18","SPECTRA NET"},
{"203.123.128/18","Pacific Internet Limited"},
{"203.124.16/21","Tulip"},
{"203.124.128/18","Gateway Systems"},
{"203.124.192/18","Tata Internet Services Ltd"},
{"203.129.192/18","STPI"},
{"203.132.128/19","Comsat Max Limited"},
{"203.134.192/19","ECL Telecom"},
{"203.145.128/18","Airtel"},
{"203.152.128/19","Emmsons Infotech"},
{"203.153.32/20","RailTel"},
{"203.153.52/22","NOVANET"},
{"203.163.128/18","Gujarat Narmada Valley Fertilizers"},
{"203.163.224/19","Hathway"},
{"203.171.210/23","ICICI BANK"},
{"203.176.112/23","Centre For Railway Information Systems"},
{"203.187.132.0/24","Flextronics"},
{"203.187.192/18","You tele"},
{"203.189.5.0/24","Ashok leyland Ltd"},
{"203.189.92.0/24","ICICI Bank"},
{"203.190.128/19","STPI"},
{"203.192.192/18","You tele"},
{"203.193.128/18","STPI"},
{"203.194.96.0/20","You tele"},
{"203.196.128/18","Primus"},
{"203.196.192/18","Tata Internet Services Ltd"},
{"203.197/16","TATA Communications"},
{"203.199/16","TATA Communications"},
{"203.200/16","TATA Communications"},
{"203.201.192/18","TATA Communications"},
{"203.212.192/18","Hathway"},
{"204.79.197/24","Microsoft"},
{"205.147.108.0/22","E2E Networks"},
{"206.183.111/24","Web Werks India"},
{"209.58.0.0/17","TATA Communications"},
{"209.85.128.0/17","Google"},
{"210.5.200/22","Primus"},
{"210.7.64/19","Primus"},
{"210.18.0/17","SIFY"},
{"210.18.128/18","Hathway"},
{"210.56.144/21","Spidigo"},
{"210.210.0/17","SIFY"},
{"210.211.128/17","TATA Communications"},
{"210.212/16","BSNL"},
{"210.214/16","SIFY"},
{"216.6.0/17","TATA Communications"},
{"216.58.192.0/19","Google"},
{"216.239.32/19","Google"},
{"218.100.48.0/23","NIXI"},
{"218.248/16","BSNL"},
{"219.64/15","TATA Communications"},
{"219.91.128/17","Iqara"},
{"220.156.184/21","CDAC"},
{"220.224/14","Reliance"},
{"221.134/15","SIFY"},
{"221.128.128/18","Exatt Technologies"},
{"223.30.0.0/15","Sify"},
{"223.165.24.0/21","INDIATIMES"},
{"223.176.0.0/12","Airtel"},
{"223.196.0.0/16","Idea Cellular"},
{"223.224.0.0/12","Airtel"},
{"",""}
};