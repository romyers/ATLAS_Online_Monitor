from PyQt5.QtWidgets import *
from PyQt5 import QtCore, QtGui, QtWidgets
import xml.etree.ElementTree as ET


class Ui_Dialog(QWidget):

    def __init__(self, TDC_inst):
        super().__init__()
        self.TDC_inst = TDC_inst
        self.delay_list = []
        self.enable_finetune_list = []
        self.DS_list = []
        self.freq_list = []
        self.PE_strength_list = []
        self.PE_mode_list = []
        self.PE_width_list = []
        self.add_and_reg = []

    def setupUi(self, Dialog):
        tree_ePortTX = ET.parse('LpGBT_transfer.xml')
        root = tree_ePortTX.getroot()

        Dialog.setObjectName("Dialog")
        Dialog.resize(623, 761)
        self.gridLayoutWidget = QtWidgets.QWidget(Dialog)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(50, 30, 560, 611))
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)

        self.clocks = 4
        row = 0
        column = 0

        for i in range(self.clocks):
            if column == 6:
                column = 0
                row += 9

            self.label = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label.setText("PS%i" % i)
            self.gridLayout.addWidget(self.label, row, column, 1, 1)

            self.label_5 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_5.setText("Frequency")
            self.gridLayout.addWidget(self.label_5, row + 1, column, 1, 1)

            self.combobox = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.combobox.addItem("disabled")
            self.combobox.addItem("40")
            self.combobox.addItem("80")
            self.combobox.addItem("160")
            self.combobox.addItem("320")
            self.combobox.addItem("640")
            self.combobox.addItem("1280")
            self.gridLayout.addWidget(self.combobox, row + 1, column + 1, 1, 1)
            self.freq_list.append(self.combobox)
            self.combobox.address = 92 + 3*i
            self.combobox.currentIndexChanged.connect(self.return_byte)

            self.label_3 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_3.setText("Fine Tune")
            self.gridLayout.addWidget(self.label_3, row + 2, column, 1, 1)

            self.checkbox = QtWidgets.QCheckBox(self.gridLayoutWidget)
            self.gridLayout.addWidget(self.checkbox, row + 2, column + 1, 1, 1)
            self.checkbox.address = 92 + 3*i
            self.enable_finetune_list.append(self.checkbox)
            self.checkbox.clicked.connect(self.return_byte)

            self.label_2 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_2.setText("Delay")
            self.gridLayout.addWidget(self.label_2, row + 3, column, 1, 1)

            self.lineEdit = QtWidgets.QLineEdit(self.gridLayoutWidget)
            self.lineEdit.setText('00000000')
            self.gridLayout.addWidget(self.lineEdit, row + 3, column + 1, 1, 1)
            self.delay_list.append(self.lineEdit)
            self.lineEdit.address = 92 + 3*i
            # self.lineEdit.textChanged.connect(self.return_byte)
            self.lineEdit.textChanged.connect(self.return_byte_2)

            self.label_4 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_4.setText("Drive Strength")
            self.gridLayout.addWidget(self.label_4, row + 4, column, 1, 1)

            self.combobox_2 = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.combobox_2.addItem("0")
            self.combobox_2.addItem("1.0")
            self.combobox_2.addItem("1.5")
            self.combobox_2.addItem("2.0")
            self.combobox_2.addItem("2.5")
            self.combobox_2.addItem("3.0")
            self.combobox_2.addItem("3.5")
            self.combobox_2.addItem("4.0")
            self.gridLayout.addWidget(self.combobox_2, row + 4, column + 1, 1, 1)
            self.DS_list.append(self.combobox_2)
            self.combobox_2.address = 92 + 3*i
            self.combobox_2.currentIndexChanged.connect(self.return_byte)

            self.label_6 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_6.setText("PE Strength")
            self.gridLayout.addWidget(self.label_6, row + 5, column, 1, 1)

            self.combobox_3 = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.combobox_3.addItem("0")
            self.combobox_3.addItem("1.0")
            self.combobox_3.addItem("1.5")
            self.combobox_3.addItem("2.0")
            self.combobox_3.addItem("2.5")
            self.combobox_3.addItem("3.0")
            self.combobox_3.addItem("3.5")
            self.combobox_3.addItem("4.0")
            self.gridLayout.addWidget(self.combobox_3, row + 5, column + 1, 1, 1)
            self.PE_strength_list.append(self.combobox_3)
            self.combobox_3.address = 94 + 3*i
            self.combobox_3.currentIndexChanged.connect(self.return_byte_3)

            self.label_7 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_7.setText("PE Mode")
            self.gridLayout.addWidget(self.label_7, row + 6, column, 1, 1)

            self.combobox_4 = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.combobox_4.addItem("disabled")
            self.combobox_4.addItem("disabled")
            self.combobox_4.addItem("self timed")
            self.combobox_4.addItem("clock timed")
            self.gridLayout.addWidget(self.combobox_4, row + 6, column + 1, 1, 1)
            self.PE_mode_list.append(self.combobox_4)
            self.combobox_4.address = 94 + 3*i
            self.combobox_4.currentIndexChanged.connect(self.return_byte_3)

            self.label_8 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_8.setText("PE Width")
            self.gridLayout.addWidget(self.label_8, row + 7, column, 1, 1)

            self.combobox_5 = QtWidgets.QComboBox(self.gridLayoutWidget)
            self.combobox_5.addItem("120")
            self.combobox_5.addItem("240")
            self.combobox_5.addItem("360")
            self.combobox_5.addItem("480")
            self.combobox_5.addItem("600")
            self.combobox_5.addItem("720")
            self.combobox_5.addItem("840")
            self.combobox_5.addItem("960")
            self.gridLayout.addWidget(self.combobox_5, row + 7, column + 1, 1, 1)
            self.PE_width_list.append(self.combobox_5)
            self.combobox_5.address = 94 + 3 * i
            self.combobox_5.currentIndexChanged.connect(self.return_byte_3)

            self.label_9 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_9.setText("  ")
            self.gridLayout.addWidget(self.label_9, row + 8, column, 1, 1)

            self.label_10 = QtWidgets.QLabel(self.gridLayoutWidget)
            self.label_10.setText("    ")
            self.gridLayout.addWidget(self.label_10, row + 6, column + 2, 1, 1)

            column += 3

        self.horizontalLayoutWidget = QtWidgets.QWidget(Dialog)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(50, 660, 521, 80))
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)

        # Apply
        self.pushButton = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton.setText("Apply")
        self.horizontalLayout.addWidget(self.pushButton)

        #OK
        self.pushButton_2 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_2.setText("OK")
        self.horizontalLayout.addWidget(self.pushButton_2)
        self.pushButton_2.clicked.connect(self.save)
        self.pushButton_2.clicked.connect(Dialog.reject)

        #Cancel
        self.pushButton_3 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_3.setText("Cancel")
        self.horizontalLayout.addWidget(self.pushButton_3)
        self.pushButton_3.clicked.connect(Dialog.reject)

    def return_byte(self):
        sender = self.sender()
        address = sender.address

        for delay, finetune, DS, freq in zip(self.delay_list, self.enable_finetune_list, self.DS_list, self.freq_list):
            if sender in (delay, finetune, DS, freq):
                if finetune.isChecked():
                    finetune_bin = 1
                else:
                    finetune_bin = 0

                freq_bin = bin(freq.currentIndex()).replace("0b", "")
                freq_bin = freq_bin[::-1]
                while len(freq_bin) < 3:
                    freq_bin += '0'
                freq_bin = freq_bin[::-1]

                DS_bin = bin(DS.currentIndex()).replace("0b", "")
                DS_bin = DS_bin[::-1]
                while len(DS_bin) < 3:
                    DS_bin += '0'
                DS_bin = DS_bin[::-1]

                delay_bin = delay.text()
                delay_bin = delay_bin[::-1]
                while len(delay_bin) < 8:
                    delay_bin += '0'
                delay_bin = delay_bin[::-1]
                delay_bin_0 = delay_bin[0]

                register_bin_value = (str(delay_bin_0) + str(finetune_bin) + DS_bin + freq_bin)
                register_int_value = int(register_bin_value, 2)
                # print(register_bin_value)
                print("%s, %s" % (address, register_int_value))

                return address, register_int_value

    def return_byte_2(self):
        sender = self.sender()
        address = sender.address + 1
        for delay in self.delay_list:
            if sender == delay:
                delay_bin = delay.text()
                delay_bin = delay_bin[::-1]
                while len(delay_bin) < 8:
                    delay_bin += '0'
                delay_bin = delay_bin[::-1]

                register_bin_value = str(delay_bin)
                register_int_value = int(register_bin_value, 2)
                print("%s, %s" % (address, register_int_value))

                return address, register_int_value

    def return_byte_3(self):
        sender = self.sender()
        address = sender.address

        for PE_s, PE_m, PE_w in zip(self.PE_strength_list, self.PE_mode_list, self.PE_width_list):
            if sender in (PE_s, PE_m, PE_w):
                PE_s_bin = bin(PE_s.currentIndex()).replace("0b", "")
                PE_s_bin = PE_s_bin[::-1]
                while len(PE_s_bin) < 3:
                    PE_s_bin += '0'
                PE_s_bin = PE_s_bin[::-1]

                PE_m_bin = bin(PE_m.currentIndex()).replace("0b", "")
                PE_m_bin = PE_m_bin[::-1]
                while len(PE_m_bin) < 2:
                    PE_m_bin += '0'
                PE_m_bin = PE_m_bin[::-1]
                print(PE_m_bin)

                PE_w_bin = bin(PE_w.currentIndex()).replace("0b", "")
                PE_w_bin = PE_w_bin[::-1]
                while len(PE_w_bin) < 3:
                    PE_w_bin += '0'
                PE_w_bin = PE_w_bin[::-1]

                register_bin_value = (PE_s_bin + PE_m_bin + PE_w_bin)
                register_int_value = int(register_bin_value, 2)
                # print(register_bin_value)
                print("%s, %s" % (address, register_int_value))

                return address, register_int_value

    def save(self):
        i = 0
        for delay, finetune, DS, freq in zip(self.delay_list, self.enable_finetune_list, self.DS_list, self.freq_list):
            address = 92 + 3*i
            if finetune.isChecked():
                finetune_bin = 1
            else:
                finetune_bin = 0

            freq_bin = bin(freq.currentIndex()).replace("0b", "")
            freq_bin = freq_bin[::-1]
            while len(freq_bin) < 3:
                freq_bin += '0'
            freq_bin = freq_bin[::-1]

            DS_bin = bin(DS.currentIndex()).replace("0b", "")
            DS_bin = DS_bin[::-1]
            while len(DS_bin) < 3:
                DS_bin += '0'
            DS_bin = DS_bin[::-1]

            delay_bin = delay.text()
            delay_bin = delay_bin[::-1]
            while len(delay_bin) < 8:
                delay_bin += '0'
            delay_bin = delay_bin[::-1]
            delay_bin_0 = delay_bin[0]

            register_bin_value = (str(delay_bin_0) + str(finetune_bin) + DS_bin + freq_bin)
            register_int_value = int(register_bin_value, 2)
            i += 1

            print("%s, %s" % (address, register_int_value))
            self.add_and_reg.append([address, register_int_value])

        # delay total register! Check on this again to be sure
        i = 0
        for delay in self.delay_list:
            address = 93 + 3*i
            delay_bin = delay.text()
            delay_bin = delay_bin[::-1]
            while len(delay_bin) < 8:
                delay_bin += '0'
            delay_bin = delay_bin[::-1]

            register_bin_value = str(delay_bin)
            register_int_value = int(register_bin_value, 2)
            print(register_bin_value)
            print("%s, %s" % (address, register_int_value))
            self.add_and_reg.append([address, register_int_value])

            i += 1
        # return address, register_int_value
        i = 0
        for PE_s, PE_m, PE_w in zip(self.PE_strength_list, self.PE_mode_list, self.PE_width_list):
            address = 94 + 3*i
            PE_s_bin = bin(PE_s.currentIndex()).replace("0b", "")
            PE_s_bin = PE_s_bin[::-1]
            while len(PE_s_bin) < 3:
                PE_s_bin += '0'
            PE_s_bin = PE_s_bin[::-1]

            PE_m_bin = bin(PE_m.currentIndex()).replace("0b", "")
            PE_m_bin = PE_m_bin[::-1]
            while len(PE_m_bin) < 2:
                PE_m_bin += '0'
            PE_m_bin = PE_m_bin[::-1]

            PE_w_bin = bin(PE_w.currentIndex()).replace("0b", "")
            PE_w_bin = PE_w_bin[::-1]
            while len(PE_w_bin) < 3:
                PE_w_bin += '0'
            PE_w_bin = PE_w_bin[::-1]

            register_bin_value = (PE_s_bin + PE_m_bin + PE_w_bin)
            register_int_value = int(register_bin_value, 2)
            print("%s, %s" % (address, register_int_value))
            self.add_and_reg.append([address, register_int_value])

            i += 1

        # getting the xml file buisness together
        list_to_xml = sorted(self.add_and_reg, key=lambda x: (x[0]))
        tree_ePortTX = ET.parse('registers.xml')
        root = tree_ePortTX.getroot()
        for register in list_to_xml:
            root[register[0]].set('value', '%i' % register[1])

        tree_ePortTX.write('registers_changed.xml')

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())