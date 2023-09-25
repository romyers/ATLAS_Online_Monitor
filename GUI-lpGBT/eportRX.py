from PyQt5 import QtCore, QtGui, QtWidgets
from LpGBT_functions__two_change_setup import *
import xml.etree.ElementTree as ET
from LpGBT_functions_two import *
from PyQt5.QtWidgets import *
from PyQt5 import QtCore, QtGui, QtWidgets
from xml.dom import minidom
import xml.etree.ElementTree as ET
from lpgbt_list import *
import sys
sys.path.insert(0, "./UART_py3")
from UART_py3.TDC_config_low_level_function import lpgbt_config_single_write

class Ui_Dialog(QWidget):

    def __init__(self, TDC_inst):
        super().__init__()
        self.TDC_inst = TDC_inst
        self.enable0_list = []
        self.enable1_list = []
        self.enable2_list = []
        self.enable3_list = []
        self.data_rate_list = []
        self.track_mode_list = []
        self.equalization_list = []

        self.phase_select_list = []
        self.inv_list = []
        self.ac_list = []
        self.term_list = []
        self.eq_list = []

        self.add_and_reg = []

        self.enable_list = [self.enable0_list, self.enable1_list, self.enable2_list, self.enable3_list]

    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.resize(1875, 766)
        self.gridLayoutWidget = QtWidgets.QWidget(Dialog)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(10, 10, 1855, 610))
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)

        self.groups = 7
        column = 0
        column_two = 1
        row = 0
        row_two = 1

        # loop over the groups (0-6)
        for i in range(self.groups):
            if column == 40:
                column = 0
                column_two = 1
                row += 9
                row_two += 9

            self.label = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label.setText("G%s" % i)
            self.label.setFont(QtGui.QFont("Calbri", 8))
            self.gridLayout.addWidget(self.label, row, column, 1, 1)

            self.label_2 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_2.setText("Data rate (5Gbps, 10Gbps):")
            self.gridLayout.addWidget(self.label_2, row + 1, column, 1, 5)

            self.comboBox = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.comboBox.addItem("disabled")
            self.comboBox.addItem("160 Mbps, 320 Mbps")
            self.comboBox.addItem("320 Mbps, 640 Mbps")
            self.comboBox.addItem("640 Mbps, 1280 Mbps")
            self.data_rate_list.append(self.comboBox)
            self.comboBox.address = 196 + i
            # Bit 3:2 - EPRX0DataRate[1:0] - Sets the data rate for group X.
            self.comboBox.setCurrentIndex(int(format(int(lpgbt_list[self.comboBox.address]),'b').zfill(8)[4:6], 2))
            self.comboBox.currentIndexChanged.connect(self.EPRX_Control_registers)
            self.gridLayout.addWidget(self.comboBox, row + 1, column + 5, 1, 4)

            self.label_3 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_3.setText("Tracking mode:")
            self.gridLayout.addWidget(self.label_3, row + 2, column, 1, 3)

            self.comboBox_5 = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.comboBox_5.addItem("Fixed phase")
            self.comboBox_5.addItem("Initial training")
            self.comboBox_5.addItem("Cont. phase tracking")
            self.comboBox_5.addItem("Cont. phase tracking with init. phase")
            self.track_mode_list.append(self.comboBox_5)
            self.comboBox_5.address = 196 + i
            # Bit 1:0 - EPRX0TrackMode[1:0] - Sets the phase tracking mode for group X.
            self.comboBox_5.setCurrentIndex(int(format(int(lpgbt_list[self.comboBox_5.address]),'b').zfill(8)[6:8], 2))
            self.comboBox_5.currentIndexChanged.connect(self.EPRX_Control_registers)
            self.gridLayout.addWidget(self.comboBox_5, row + 2, column + 5, 1, 4)


            self.label_4 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_4.setText("en")
            self.gridLayout.addWidget(self.label_4, row + 3, column_two, 1, 1)

            for j, self.enable_listing in zip(range(4), self.enable_list):
                self.checkBox = QtWidgets.QCheckBox(self.gridLayoutWidget)
                self.checkBox.address = 196 + i
                self.enable_listing.append(self.checkBox)
                self.gridLayout.addWidget(self.checkBox, row + 4 + j, column_two, 1, 1)
                if format(int(lpgbt_list[self.checkBox.address]),'b').zfill(8)[3 - j] == '1':
                    self.checkBox.setChecked(True)
                else:
                    self.checkBox.setChecked(False)
                self.checkBox.clicked.connect(self.EPRX_Control_registers)

            self.label_inv = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_inv.setText("inv")
            self.gridLayout.addWidget(self.label_inv, row + 3, column_two + 1, 1, 1)

            self.label_ac = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_ac.setText("ac")
            self.gridLayout.addWidget(self.label_ac, row + 3, column_two + 2, 1, 1)

            self.label_term = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_term.setText("term")
            self.gridLayout.addWidget(self.label_term, row + 3, column_two + 3, 1, 1)

            for j in range(4):
                self.checkBox_inv = QtWidgets.QCheckBox(self.gridLayoutWidget)
                self.checkBox_inv.address = 204 + j + 4*i
                if format(int(lpgbt_list[self.checkBox_inv.address]),'b').zfill(8)[4] == '1':
                    self.checkBox_inv.setChecked(True)
                else:
                    self.checkBox_inv.setChecked(False)
                self.checkBox_inv.clicked.connect(self.EPRX_chn_cntr)
                self.gridLayout.addWidget(self.checkBox_inv, row + 4 + j, column_two + 1, 1, 1)
                self.inv_list.append(self.checkBox_inv)

                self.checkBox_ac = QtWidgets.QCheckBox(self.gridLayoutWidget)
                self.checkBox_ac.address = 204 + j + 4*i
                if format(int(lpgbt_list[self.checkBox_ac.address]),'b').zfill(8)[5] == '1':
                    self.checkBox_ac.setChecked(True)
                else:
                    self.checkBox_ac.setChecked(False)
                self.checkBox_ac.clicked.connect(self.EPRX_chn_cntr)
                self.gridLayout.addWidget(self.checkBox_ac, row + 4 + j, column_two + 2, 1, 1)
                self.ac_list.append(self.checkBox_ac)

                self.checkBox_term = QtWidgets.QCheckBox(self.gridLayoutWidget)
                self.checkBox_term.address = 204 + j + 4*i
                if format(int(lpgbt_list[self.checkBox_term.address]),'b').zfill(8)[6] == '1':
                    self.checkBox_term.setChecked(True)
                else:
                    self.checkBox_term.setChecked(False)
                self.checkBox_term.clicked.connect(self.EPRX_chn_cntr)
                self.gridLayout.addWidget(self.checkBox_term, row + 4 + j, column_two + 3, 1, 1)
                self.term_list.append(self.checkBox_term)

            self.label_5 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_5.setText("phase")
            self.gridLayout.addWidget(self.label_5, row + 3, column_two + 4, 1, 1)

            self.label_space = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_space.setText(" ")
            self.gridLayout.addWidget(self.label_space, row + 3, column_two + 5, 1, 1)

            for j in range(4):
                self.spinBox_2 = QtWidgets.QSpinBox(self.gridLayoutWidget)
                self.spinBox_2.setMaximum(15)
                self.spinBox_2.address = 204 + j + 4*i
                self.phase_select_list.append(self.spinBox_2)
                self.spinBox_2.setValue(int(format(int(lpgbt_list[self.spinBox_2.address]),'b').zfill(8)[0:4]))
                self.spinBox_2.valueChanged.connect(self.EPRX_chn_cntr)
                self.gridLayout.addWidget(self.spinBox_2, row + 4 + j, column_two + 4, 1, 2)

            self.label_8 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_8.setText("eql")
            self.gridLayout.addWidget(self.label_8, row + 3, column_two + 6, 1, 1)

            for j in range(4):
                self.comboBox_2 = QtWidgets.QComboBox(self.gridLayoutWidget)
                self.comboBox_2.addItem("N/A")
                self.comboBox_2.addItem("300 MHz, 4.9 dB")
                self.comboBox_2.addItem("125 MHz, 7.8 dB")
                self.comboBox_2.addItem("70 MHz, 10.7 dB")
                self.eq_list.append(self.comboBox_2)
                self.comboBox_2.address = 204 + j + 4*i
                part_1 = format(int(lpgbt_list[self.comboBox_2.address]),'b').zfill(8)[7]
                part_2 = ''
                if i in (0, 1):
                    self.comboBox_2.address_2 = 233
                    part_2 = format(int(lpgbt_list[self.comboBox_2.address]),'b').zfill(8)[7 - j - 4*i]
                if i in (2, 3):
                    self.comboBox_2.address_2 = 234
                    part_2 = format(int(lpgbt_list[self.comboBox_2.address]),'b').zfill(8)[7 - j - 4 * (i - 2)]     ## Checking to see if this works out!
                if i in (4, 5):
                    self.comboBox_2.address_2 = 235
                    part_2 = format(int(lpgbt_list[self.comboBox_2.address]),'b').zfill(8)[7 - j - 4 * (i - 4)]
                if i == 6:
                    self.comboBox_2.address_2 = 236
                    part_2 = format(int(lpgbt_list[self.comboBox_2.address]),'b').zfill(8)[7 - j]
                total = part_1 + part_2
                self.comboBox_2.setCurrentIndex(int(total, 2))
                # self.comboBox_2.currentIndexChanged.connect(self.EQ_control)
                self.comboBox_2.currentIndexChanged.connect(self.EPRX_chn_cntr)
                self.gridLayout.addWidget(self.comboBox_2, row + 4 + j, column_two + 6, 1, 1)

            self.label_10 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_10.setText("Ch0:")
            self.gridLayout.addWidget(self.label_10, row + 4, column, 1, 1)

            self.label_11 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_11.setText("Ch1:")
            self.gridLayout.addWidget(self.label_11, row + 5, column, 1, 1)

            self.label_12 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_12.setText("Ch2:")
            self.gridLayout.addWidget(self.label_12, row + 6, column, 1, 1)

            self.label_13 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_13.setText("Ch3:")
            self.gridLayout.addWidget(self.label_13, row + 7, column, 1, 1)

            self.label_14 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_14.setText("          ")
            self.gridLayout.addWidget(self.label_14, row + 3, column, 1, 1)

            column += 10
            column_two += 10
##########################################################################
        # EC channel stuff
        # Configuration of the EC channel in ePortRx
        # Bit 7:4 - EPRXECPhaseSelect[3:0] - Static phase for the EC channel.
        # Bit 3 - EPRXECInvert - Inverts the EC channel data input.
        # Bit 2 - EPRXECAcBias - Enables the common mode generation for the EC channel.
        # Bit 1 - EPRXECTerm - Enables the 100 Ohm termination for EC channel.
        # Bit 0 - EPRXECEnable - Enables the EC channel.
        ec_column = 31
        ec_row = 12
        self.label_15 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_15.setText("Ec Channel:")
        self.gridLayout.addWidget(self.label_15, ec_row-3, ec_column-1, 1, 1)     

        self.label_16 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_16.setText("en")
        self.gridLayout.addWidget(self.label_16, ec_row, ec_column, 1, 1)

        self.label_ec_inv = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_ec_inv.setText("inv")
        self.gridLayout.addWidget(self.label_ec_inv, ec_row, ec_column+1, 1, 1)

        self.label_ec_ac = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_ec_ac.setText("ac")
        self.gridLayout.addWidget(self.label_ec_ac, ec_row, ec_column+2, 1, 1)

        self.label_16 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_16.setText("term")
        self.gridLayout.addWidget(self.label_16, ec_row, ec_column+3, 1, 1)

        self.checkBox_en_ec = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.checkBox_en_ec.address = 232
        if format(int(lpgbt_list[self.checkBox_en_ec.address]),'b').zfill(8)[7] == '1':
            self.checkBox_en_ec.setChecked(True)
        else:
            self.checkBox_en_ec.setChecked(False)
        self.checkBox_en_ec.clicked.connect(self.EC_chn_cntr)
        self.gridLayout.addWidget(self.checkBox_en_ec, ec_row+1, ec_column, 1, 1)


        self.checkBox_inv_ec = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.checkBox_inv_ec.address = 232
        if format(int(lpgbt_list[self.checkBox_inv_ec.address]),'b').zfill(8)[4] == '1':
            self.checkBox_inv_ec.setChecked(True)
        else:
            self.checkBox_inv_ec.setChecked(False)
        self.checkBox_inv_ec.clicked.connect(self.EC_chn_cntr)
        self.gridLayout.addWidget(self.checkBox_inv_ec, ec_row+1, ec_column + 1, 1, 1)
        self.inv_list.append(self.checkBox_inv_ec)

        self.checkBox_ac_ec = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.checkBox_ac_ec.address = 232
        if format(int(lpgbt_list[self.checkBox_ac_ec.address]),'b').zfill(8)[5] == '1':
            self.checkBox_ac_ec.setChecked(True)
        else:
            self.checkBox_ac_ec.setChecked(False)
        self.checkBox_ac_ec.clicked.connect(self.EC_chn_cntr)
        self.gridLayout.addWidget(self.checkBox_ac_ec, ec_row+1, ec_column + 2, 1, 1)
        self.ac_list.append(self.checkBox_ac_ec)

        self.checkBox_term_ec = QtWidgets.QCheckBox(self.gridLayoutWidget)
        self.checkBox_term_ec.address = 232
        if format(int(lpgbt_list[self.checkBox_term_ec.address]),'b').zfill(8)[6] == '1':
            self.checkBox_term_ec.setChecked(True)
        else:
            self.checkBox_term_ec.setChecked(False)
        self.checkBox_term_ec.clicked.connect(self.EC_chn_cntr)
        self.gridLayout.addWidget(self.checkBox_term_ec, ec_row+1, ec_column + 3, 1, 1)
        self.term_list.append(self.checkBox_term_ec)



        self.label_17 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_17.setText("Tracking Mode:")
        self.gridLayout.addWidget(self.label_17, ec_row-1, ec_column, 1, 3)

        self.comboBox_6 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_6.address = 203
        self.comboBox_6.addItem("Fixed phase")
        self.comboBox_6.addItem("Initial training")
        self.comboBox_6.addItem("Continuous phase tracking")
        self.comboBox_6.addItem("Continuous phase tracking with initial phase")        
        self.comboBox_6.setCurrentIndex(int(format(int(lpgbt_list[self.comboBox_6.address]),'b').zfill(8)[6:8], 2))
        self.comboBox_6.currentIndexChanged.connect(self.EC_control_register)
        self.gridLayout.addWidget(self.comboBox_6, ec_row-1, ec_column+4, 1, 4)


        self.label_18 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_18.setText("phase")
        self.gridLayout.addWidget(self.label_18, ec_row, ec_column+4, 1, 1)

        self.spinBox_4 = QtWidgets.QSpinBox(self.gridLayoutWidget)
        self.spinBox_4.address = 232
        self.spinBox_4.setMaximum(15)
        self.gridLayout.addWidget(self.spinBox_4, ec_row+1, ec_column+4, 1, 2)
        self.spinBox_4.setValue(int(format(int(lpgbt_list[self.spinBox_4.address]),'b').zfill(8)[0:4], 2))
        self.spinBox_4.valueChanged.connect(self.EC_chn_cntr)

        self.horizontalLayoutWidget = QtWidgets.QWidget(Dialog)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(110, 660, 1671, 80))
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)

        self.pushButton_3 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_3.setText("Apply")
        self.pushButton_3.clicked.connect(self.save)
        self.horizontalLayout.addWidget(self.pushButton_3)

        self.pushButton_2 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_2.setText("OK")
        self.horizontalLayout.addWidget(self.pushButton_2)
        self.pushButton_2.clicked.connect(self.save)
        self.pushButton_2.clicked.connect(Dialog.reject)

        self.pushButton = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton.setText("Cancel")
        self.horizontalLayout.addWidget(self.pushButton)
        self.pushButton.clicked.connect(Dialog.reject)

    def EPRX_Control_registers(self):
        sender = self.sender()
        address = sender.address

        for enbl0, enbl1, enbl2, enbl3, datarate, trackmode in zip(self.enable0_list, self.enable1_list, self.enable2_list, self.enable3_list, self.data_rate_list, self.track_mode_list):
            if sender in (enbl0, enbl1, enbl2, enbl3, datarate, trackmode):
                enbl0_bin = '1' if enbl0.isChecked() else '0'
                enbl1_bin = '1' if enbl1.isChecked() else '0'
                enbl2_bin = '1' if enbl2.isChecked() else '0'
                enbl3_bin = '1' if enbl3.isChecked() else '0'
                datarate_bin = format(datarate.currentIndex(),'b').zfill(2)
                trackmode_bin = format(trackmode.currentIndex(),'b').zfill(2)
                register_bin_value = enbl3_bin + enbl2_bin + enbl1_bin + enbl0_bin + datarate_bin + trackmode_bin
                register_int_value = int(register_bin_value, 2)

                print("Writing to address=%s,value=%s" % (address, register_int_value))
                lpgbt_config_single_write(self.TDC_inst.ser, 117, address, register_int_value)
                return address, register_int_value, register_bin_value

    def EC_control_register(self):
        # sender = self.sender()
        address = 203
        EC_trackmode_bin = format(self.comboBox_6.currentIndex(),'b').zfill(2)
        register_bin_value = EC_trackmode_bin
        register_int_value = int(register_bin_value, 2)

        print("Writing to address=%s,value=%s" % (address, register_int_value))
        lpgbt_config_single_write(self.TDC_inst.ser, 117, address, register_int_value)
        return address, register_int_value, register_bin_value

    def EPRX_chn_cntr(self):
        sender = self.sender()
        address = sender.address
        invert_bin = '0'
        AcBias_bin = '0'
        Term_bin = '0'

        for phase, inv, ac, term, eq in zip(self.phase_select_list, self.inv_list, self.ac_list, self.term_list, self.eq_list):
            if sender in (phase, inv, ac, term, eq):
                phase_bin = format(phase.value(),'b').zfill(4)
                invert_bin = "1" if inv.isChecked() else "0"
                AcBias_bin = "1" if ac.isChecked() else "0"
                Term_bin = "1" if term.isChecked() else "0"
                eq_bin_leading_bit = format(eq.currentIndex(),'b').zfill(2)[0]

                register_bin_value = phase_bin + invert_bin + AcBias_bin + Term_bin + eq_bin_leading_bit
                register_int_value = int(register_bin_value, 2)

                print("Writing to address=%s,value=%s" % (address, register_int_value))
                lpgbt_config_single_write(self.TDC_inst.ser, 117, address, register_int_value)
                

                # [0x0e9] EPRXEq10Control
                # Bit 7 - EPRX13Eq[0] - Equalization control for channel 3 in group 1.
                # Bit 6 - EPRX12Eq[0] - Equalization control for channel 2 in group 1.
                # Bit 5 - EPRX11Eq[0] - Equalization control for channel 1 in group 1.
                # Bit 4 - EPRX10Eq[0] - Equalization control for channel 0 in group 1.
                # Bit 3 - EPRX03Eq[0] - Equalization control for channel 3 in group 0.
                # Bit 2 - EPRX02Eq[0] - Equalization control for channel 2 in group 0.
                # Bit 1 - EPRX01Eq[0] - Equalization control for channel 1 in group 0.
                # Bit 0 - EPRX00Eq[0] - Equalization control for channel 0 in group 0.

                if sender == eq: # additional writing to eq register
                    address = sender.address_2
                    if address in (233,234,235):
                        start = (address-233)*8+7
                        end = start - 8
                    elif address == 236:
                        start = (address-233)*8+3
                        end = start - 4
                    eq_bin = ''
                    print("eq_list len="+str(len(self.eq_list)))
                    for i in range (start,end,-1):
                        eq_bin += format(self.eq_list[i].currentIndex(),'b').zfill(2)[1]
                    register_bin_value = eq_bin
                    register_int_value = int(register_bin_value, 2)
                    print("Writing to address=%s,value=%s" % (address, register_int_value))
                    lpgbt_config_single_write(self.TDC_inst.ser, 117, address, register_int_value)
                return address, register_int_value, register_bin_value



    def EC_chn_cntr(self):
        address = 232
        phase_bin = format(self.spinBox_4.value(),'b').zfill(4)
        invert_bin = "1" if self.checkBox_inv_ec.isChecked() else "0"
        AcBias_bin = "1" if self.checkBox_ac_ec.isChecked() else "0"
        Term_bin = "1" if self.checkBox_term_ec.isChecked() else "0"
        enable_bin = "1" if self.checkBox_en_ec.isChecked() else "0"
        register_bin_value = phase_bin + invert_bin + AcBias_bin + Term_bin + enable_bin
        register_int_value = int(register_bin_value, 2)
        print("Writing to address=%s,value=%s" % (address, register_int_value))
        lpgbt_config_single_write(self.TDC_inst.ser, 117, address, register_int_value)
        return address, register_int_value, register_bin_value



    def save(self):
        print("Save all page parameters")
        ###############################################################################################################
        # EPRX control registers
        i = 0
        for enbl0, enbl1, enbl2, enbl3, datarate, trackmode in zip(self.enable0_list, self.enable1_list, self.enable2_list, self.enable3_list, self.data_rate_list, self.track_mode_list):
            address = 196 + i
            enbl0_bin = "1" if enbl0.isChecked() else "0"
            enbl1_bin = "1" if enbl1.isChecked() else "0"
            enbl2_bin = "1" if enbl2.isChecked() else "0"
            enbl3_bin = "1" if enbl3.isChecked() else "0"
            datarate_bin = format(datarate.currentIndex(),'b').zfill(2)
            trackmode_bin = format(trackmode.currentIndex(),'b').zfill(2)
            register_bin_value = enbl3_bin + enbl2_bin + enbl1_bin + enbl0_bin + datarate_bin + trackmode_bin
            register_int_value = int(register_bin_value, 2)

            self.add_and_reg.append([address, register_int_value])
            list_update(address, register_bin_value)
            i += 1

        ###########################################################################################################
        # EC control register
        address = 203
        EC_trackmode_bin = format(self.comboBox_6.currentIndex(),'b').zfill(2)
        register_bin_value = EC_trackmode_bin
        register_int_value = int(register_bin_value, 2)
        self.add_and_reg.append([address, register_int_value])
        list_update(address, register_bin_value)


        #####################################################################################################
        # EPRX_chn_cntr
        invert_bin = '0'
        AcBias_bin = '0'
        Term_bin = '0'
        address_list = [i for i in range (204,204+7*4)]
        for phase, inv, ac, term, eq, address in zip(self.phase_select_list, self.inv_list, self.ac_list, self.term_list, self.eq_list, address_list):
            phase_bin = format(phase.value(),'b').zfill(4)
            invert_bin = "1" if inv.isChecked() else "0"
            AcBias_bin = "1" if ac.isChecked() else "0"
            Term_bin = "1" if term.isChecked() else "0"
            eq_bin_leading_bit = format(eq.currentIndex(),'b').zfill(2)[0]
            register_bin_value = phase_bin + invert_bin + AcBias_bin + Term_bin + eq_bin_leading_bit
            register_int_value = int(register_bin_value, 2)
            self.add_and_reg.append([address, register_int_value])
            list_update(address, register_bin_value)

        ##########################################################################
        # EC_chn_cntr
        address = 232
        phase_bin = format(self.spinBox_4.value(),'b').zfill(4)
        invert_bin = "1" if self.checkBox_inv_ec.isChecked() else "0"
        AcBias_bin = "1" if self.checkBox_ac_ec.isChecked() else "0"
        Term_bin = "1" if self.checkBox_term_ec.isChecked() else "0"
        enable_bin = "1" if self.checkBox_en_ec.isChecked() else "0"
        register_bin_value = phase_bin + invert_bin + AcBias_bin + Term_bin + enable_bin
        register_int_value = int(register_bin_value, 2)
        self.add_and_reg.append([address, register_int_value])
        list_update(address, register_bin_value)

     ##################################################################################
        # EQ_control
        address_list = [233, 234, 235, 236]
        ##### Working on getting the right parameters to change with it!!!!
        for address in address_list:
            if address in (233,234,235):
                start = (address-233)*8+7
                end = start - 8
            elif address == 236:
                start = (address-233)*8+3
                end = start - 4
            eq_bin = ''
            for i in range (start,end,-1):
                eq_bin += format(self.eq_list[i].currentIndex(),'b').zfill(2)[1]
            register_bin_value = eq_bin
            register_int_value = int(register_bin_value, 2)
            self.add_and_reg.append([address, register_int_value])
            list_update(address, register_bin_value)

        # write value to xml
        tree_ePortTX = ET.parse('registers_changed.xml')
        root = tree_ePortTX.getroot()
        self.list_to_xml = sorted(self.add_and_reg, key=lambda x: (x[0]))
        for register in self.list_to_xml:
            root[register[0]].set('value', '%i' % register[1])
        tree_ePortTX.write('registers_changed.xml')

