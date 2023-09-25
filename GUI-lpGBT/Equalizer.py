import xml.etree.ElementTree as ET
from LpGBT_functions_two import *
from PyQt5.QtWidgets import *
from PyQt5 import QtCore, QtGui, QtWidgets
from xml.dom import minidom
import xml.etree.ElementTree as ET

class Ui_Dialog(QWidget):

    def __init__(self, TDC_inst):
        super().__init__()
        self.TDC_inst = TDC_inst
        self.add_and_reg = []
        self.address = 0

    def setupUi(self, Dialog):

        tree_ePortTX = ET.parse('LpGBT_transfer.xml')
        root = tree_ePortTX.getroot()

        Dialog.setObjectName("Dialog")
        Dialog.resize(434, 388)
        Dialog.setWindowTitle("Equalizer")
        self.gridLayoutWidget = QtWidgets.QWidget(Dialog)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(30, 50, 371, 211))
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.label_3 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_3.setText("Resistance 3 (kOhm):")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.label = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label.setText("Attenuation (V/V):")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.label_2 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_2.setText("Capacitance (fF):")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.label_6 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_6.setText("Resistance 0 (kOhm):")
        self.gridLayout.addWidget(self.label_6, 5, 0, 1, 1)
        self.label_5 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_5.setText("Resistance 1 (kOhm):")
        self.gridLayout.addWidget(self.label_5, 4, 0, 1, 1)
        self.label_4 = QtWidgets.QLabel(self.gridLayoutWidget)
        self.label_4.setText("Resistance 2 (kOhm):")
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)

        self.comboBox = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox.addItem("1/3")
        self.comboBox.addItem("2/3")
        self.comboBox.addItem("1/1")
        self.comboBox.address = 55
        self.gridLayout.addWidget(self.comboBox, 0, 1, 1, 1)
        self.comboBox.currentIndexChanged.connect(self.return_byte)

        self.comboBox_2 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_2.addItem("0")
        self.comboBox_2.addItem("70")
        self.comboBox_2.addItem("70")
        self.comboBox_2.addItem("140")
        self.comboBox_2.address = 55
        self.gridLayout.addWidget(self.comboBox_2, 1, 1, 1, 1)
        self.comboBox_2.currentIndexChanged.connect(self.return_byte)

        self.comboBox_3 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_3.addItem("0")
        self.comboBox_3.addItem("0.4")
        self.comboBox_3.addItem("1.0")
        self.comboBox_3.addItem("1.6")
        self.comboBox_3.address = 56
        self.gridLayout.addWidget(self.comboBox_3, 2, 1, 1, 1)
        self.comboBox_3.currentIndexChanged.connect(self.return_byte_2)

        self.comboBox_7 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_7.addItem("0")
        self.comboBox_7.addItem("0.6")
        self.comboBox_7.addItem("1.2")
        self.comboBox_7.addItem("2.4")
        self.comboBox_7.address = 56
        self.gridLayout.addWidget(self.comboBox_7, 3, 1, 1, 1)
        self.comboBox_7.currentIndexChanged.connect(self.return_byte_2)

        self.comboBox_8 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_8.addItem("0")
        self.comboBox_8.addItem("3.0")
        self.comboBox_8.addItem("4.9")
        self.comboBox_8.addItem("7.1")
        self.comboBox_8.address = 56
        self.gridLayout.addWidget(self.comboBox_8, 4, 1, 1, 1)
        self.comboBox_8.currentIndexChanged.connect(self.return_byte_2)

        self.comboBox_9 = QtWidgets.QComboBox(self.gridLayoutWidget)
        self.comboBox_9.addItem("0")
        self.comboBox_9.addItem("3.0")
        self.comboBox_9.addItem("4.9")
        self.comboBox_9.addItem("7.1")
        self.comboBox_9.address = 56
        self.gridLayout.addWidget(self.comboBox_9, 5, 1, 1, 1)
        self.comboBox_9.currentIndexChanged.connect(self.return_byte_2)

        self.label_7 = QtWidgets.QLabel(Dialog)
        self.label_7.setGeometry(QtCore.QRect(30, 20, 181, 21))
        self.label_7.setText("Equalizer: ")
        self.horizontalLayoutWidget = QtWidgets.QWidget(Dialog)
        self.horizontalLayoutWidget.setGeometry(QtCore.QRect(30, 290, 371, 71))
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.horizontalLayoutWidget)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)

        self.pushButton = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton.setText("Apply")
        self.horizontalLayout.addWidget(self.pushButton)
        self.pushButton.clicked.connect(self.save)
        self.pushButton_2 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_2.setText("OK")
        self.horizontalLayout.addWidget(self.pushButton_2)
        self.pushButton_2.clicked.connect(self.save)
        self.pushButton_2.clicked.connect(Dialog.reject)
        self.pushButton_3 = QtWidgets.QPushButton(self.horizontalLayoutWidget)
        self.pushButton_3.setText("Cancel")
        self.horizontalLayout.addWidget(self.pushButton_3)
        self.pushButton_3.clicked.connect(Dialog.reject)

################################################################

    def return_byte(self):
        sender = self.sender()
        self.address = sender.address

        self.function_for_reg55()

    def return_byte_2(self):
        sender = self.sender()
        self.address = sender.address

        self.function_for_reg56()

    def save(self):
        print("save all page parameters")

        #  register 55
        self.address = 55
        self.function_for_reg55()
        self.add_and_reg.append([self.function_for_reg55()[0], self.function_for_reg55()[1]])
        #register 56
        self.address = 56
        self.function_for_reg56()
        self.add_and_reg.append([self.function_for_reg56()[0], self.function_for_reg56()[1]])

        #xml file - work on adding to the existing xml and sort it!
        tree_ePortTX = ET.parse('registers.xml')
        root = tree_ePortTX.getroot()

        root[55].set('value', '%i' % self.function_for_reg55()[1])
        root[56].set('value', '%i' % self.function_for_reg56()[1])

        tree_ePortTX.write('registers_changed.xml')

    def function_for_reg55(self):
        self.address = 55
        Atten_bin = bin(self.comboBox.currentIndex()).replace("0b", "")
        Atten_bin = Atten_bin[::-1]
        while len(Atten_bin) < 2:
            Atten_bin += '0'
        Atten_bin = Atten_bin[::-1]

        Cap_bin = bin(self.comboBox_2.currentIndex()).replace("0b", "")
        Cap_bin = Cap_bin[::-1]
        while len(Cap_bin) < 2:
            Cap_bin += '0'
        Cap_bin = Cap_bin[::-1]

        register_bin_value = (Atten_bin + Cap_bin)
        register_int_value = int(register_bin_value, 2)
        print(register_bin_value)
        # print(self.comboBox_2.currentIndex())

        print("Address %s : value %s" % (self.address, register_int_value))
        return self.address, register_int_value

    def function_for_reg56(self):
        self.address = 56
        bin_list = []
        Res3_bin = bin(self.comboBox_3.currentIndex()).replace("0b", "")
        Res2_bin = bin(self.comboBox_7.currentIndex()).replace("0b", "")
        Res1_bin = bin(self.comboBox_8.currentIndex()).replace("0b", "")
        Res0_bin = bin(self.comboBox_9.currentIndex()).replace("0b", "")
        Res_list = [Res3_bin, Res2_bin, Res1_bin, Res0_bin]
        for Res in Res_list:
            Res = Res[::-1]
            while len(Res) < 2:
                Res += '0'
            Res = Res[::-1]
            bin_list.append(Res)

        register_bin_value = bin_list[0] + bin_list[1] + bin_list[2] + bin_list[3]
        register_int_value = int(register_bin_value, 2)
        print(register_bin_value)

        print("Address %s : value %s" % (self.address, register_int_value))
        return self.address, register_int_value


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    Dialog = QtWidgets.QDialog()
    ui = Ui_Dialog()
    ui.setupUi(Dialog)
    Dialog.show()
    sys.exit(app.exec_())

