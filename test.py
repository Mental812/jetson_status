from jtop import jtop, JtopException
with jtop() as jetson:
    print(type(jetson.board['hardware']['CHIP_ID']))
   # print(use,tot)
