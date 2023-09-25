from PyQt5 import QtCore, QtGui, QtWidgets

from TDC_config_low_level_function import *

import time
import datetime


class sca_tab(object):
    """docstring for rad_tab"""

    def __init__(self, MainWindow, ser):
        self.sca_enable = 6
        
        self.ser = ser
        self.ADC_channel = 21
        self.setup_UI(MainWindow)
        

        

    def setup_UI(self, MainWindow):
        grid_x = 20
        grid_y = 20
        grid_width = 300
        grid_height = 50
        
        self.gridLayoutWidget = QtWidgets.QWidget(MainWindow)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(grid_x,grid_y,grid_width,grid_height))
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)

        # #checkbox labels (0-2)
        # for i in range(3):
        #     label_num = i
        #     label = QtWidgets.QLabel()
        #     self.gridLayout.addWidget(label, 0, i+1, 1, 1)
        #     label.setText("SCA%d"%i)

        # hit enable checkboxes
        self.checkbox_list = []
        for i in range(3):
            checkBox = QtWidgets.QCheckBox()
            checkBox.setText("SCA%d"%i)
            self.gridLayout.addWidget(checkBox, 1, i+1, 1, 1)
            if format(self.sca_enable,'b')[-i-1] == '1':
                checkBox.setChecked(True)
            checkBox.stateChanged.connect(self.sca_enable_update)
            self.checkbox_list.append(checkBox)

        grid_y+=grid_height
        self.gridLayoutWidget_2 = QtWidgets.QWidget(MainWindow)
        self.gridLayoutWidget_2.setGeometry(QtCore.QRect(grid_x,grid_y,grid_width,grid_height))
        self.gridLayout_2 = QtWidgets.QGridLayout(self.gridLayoutWidget_2)

        #Check all button
        self.pushButton_enable_all = QtWidgets.QPushButton()
        self.pushButton_enable_all.setText("Check All")
        self.gridLayout_2.addWidget(self.pushButton_enable_all,0,0,1,1)
        self.pushButton_enable_all.clicked.connect(self.all_sca_enable)

        #Clear all button
        self.pushButton_disable_all = QtWidgets.QPushButton()
        self.pushButton_disable_all.setText("Clear All")
        self.gridLayout_2.addWidget(self.pushButton_disable_all,0,1,1,1)
        self.pushButton_disable_all.clicked.connect(self.all_sca_disable)

        grid_y+=grid_height
        grid_height = 150
        grid_width = 450
        self.gridLayoutWidget_3 = QtWidgets.QWidget(MainWindow)
        self.gridLayoutWidget_3.setGeometry(QtCore.QRect(grid_x,grid_y,grid_width,grid_height))
        self.gridLayout_3 = QtWidgets.QGridLayout(self.gridLayoutWidget_3)

        self.pushButton_sca_connect = QtWidgets.QPushButton()
        self.pushButton_sca_connect.setText("SCA connect")
        self.gridLayout_3.addWidget(self.pushButton_sca_connect,0,0,1,1)
        self.pushButton_sca_connect.clicked.connect(self.sca_connect)

        self.pushButton_sca_reset = QtWidgets.QPushButton()
        self.pushButton_sca_reset.setText("SCA reset")
        self.gridLayout_3.addWidget(self.pushButton_sca_reset,0,1,1,1)
        self.pushButton_sca_reset.clicked.connect(self.sca_reset)

        self.pushButton_sca_tx_rx_reset = QtWidgets.QPushButton()
        self.pushButton_sca_tx_rx_reset.setText("SCA tx rx reset")
        self.gridLayout_3.addWidget(self.pushButton_sca_tx_rx_reset,0,2,1,1)
        self.pushButton_sca_tx_rx_reset.clicked.connect(self.sca_tx_rx_reset)


        self.checkBox_ADC_enable = QtWidgets.QCheckBox()
        self.checkBox_ADC_enable.setText("ADC enable")
        self.gridLayout_3.addWidget(self.checkBox_ADC_enable, 1, 0, 1, 1)
        self.checkBox_ADC_enable.stateChanged.connect(self.ADC_enable)

        label = QtWidgets.QLabel()
        self.gridLayout_3.addWidget(label, 2, 0, 1, 1)
        label.setText("ADC_channel")
        self.spinBox_ADC_channel = QtWidgets.QSpinBox(self.gridLayoutWidget_3)
        self.gridLayout_3.addWidget(self.spinBox_ADC_channel, 2, 1, 1, 1)
        self.spinBox_ADC_channel.setValue(self.ADC_channel)
        self.spinBox_ADC_channel.valueChanged.connect(self.ADC_channel_update)

        self.pushButton_ADC_go = QtWidgets.QPushButton()
        self.pushButton_ADC_go.setText("ADC go")
        self.gridLayout_3.addWidget(self.pushButton_ADC_go,2,2,1,1)
        self.pushButton_ADC_go.clicked.connect(self.ADC_go)


    def all_sca_enable(self):
        for i in self.checkbox_list:
            i.setChecked(True)


    def all_sca_disable(self):
        for i in self.checkbox_list:
            i.setChecked(False)

    def sca_connect(self):
        SCA_connect(self.ser,sca_enable=self.sca_enable)

    def sca_enable_update(self):
        enable = 0
        for index, checkbox in enumerate(self.checkbox_list):
            if checkbox.isChecked():
                enable = enable + 2**index
        self.sca_enable = enable

    def sca_reset(self):
        SCA_start_reset(self.ser,self.sca_enable)

    def sca_tx_rx_reset(self):
        SCA_tx_rx_reset(self.ser,self.sca_enable)

    def ADC_enable(self):
        if self.checkBox_ADC_enable.isChecked():
            SCA_config(self.ser,self.sca_enable,sca_address=0x00,transID=0x01,channel=0x0,datalen=4,command=0x06,data=0x10000000)
        else:
            SCA_config(self.ser,self.sca_enable,sca_address=0x00,transID=0x01,channel=0x0,datalen=4,command=0x06,data=0x00000000)

    def ADC_write_mux(self):
        SCA_config(self.ser,self.sca_enable,sca_address=0x00,transID=0x02,channel=0x14,datalen=4,command=0x50,data=self.ADC_channel)


    def ADC_channel_update(self):
        self.ADC_channel = self.spinBox_ADC_channel.value()
        SCA_config(self.ser,self.sca_enable,sca_address=0x00,transID=0x02,channel=0x14,datalen=4,command=0x50,data=self.ADC_channel)

    def ADC_go(self):
        SCA_config(self.ser,self.sca_enable,sca_address=0x00,transID=0x05,channel=0x14,datalen=4,command=0x02,data=0x01)