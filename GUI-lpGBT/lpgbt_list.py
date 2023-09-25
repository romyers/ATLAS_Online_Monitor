lpgbt_list = []
# for i in range(350):
#     lpgbt_list.append('00000000')


import xml.etree.ElementTree as ET
tree_ePortTX = ET.parse('lpgbt_list_new.xml')
root = tree_ePortTX.getroot()
for register in root:
    # print(register.attrib['value'])
    lpgbt_list.append(register.attrib['value'])
#chip config
# for i in range(3):
print("lpgbt_list load finished")
print("length="+str(len(lpgbt_list)))
print(lpgbt_list)

def list_update(address, value):
    lpgbt_list[address] = value

