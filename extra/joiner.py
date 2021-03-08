import os

final= open("build/final.csv",'w')
final.write("ARCHIVO,PREDIOS,INVERSION,POTENCIA,VAN,TIR,FLUJO_1,FLUJO_2,FLUJO_3,FLUJO_4,FLUJO_5,FLUJO_6,FLUJO_7,FLUJO_8,FLUJO_9,FLUJO_10,FLUJO_11,FLUJO_12,FLUJO_13,FLUJO_14,FLUJO_15,FLUJO_16,FLUJO_17,FLUJO_18,FLUJO_19,FLUJO_20,POTENCIAS:\n")

archs= os.listdir('build/econom')
for arch in archs:
	print arch
	fil= open("build/econom/"+arch)
	predios=0
	potencias=[]
	for lin in fil:
		lin= lin.strip()
		if predios>0:
			potencias.append(lin.split(',')[4])
		if "todos,-,-," in lin:
			lin= ",".join([arch,str(predios-1)]+lin.split(',')[3:]+potencias[:-1])
			final.write(lin+"\n")
		predios+=1
	fil.close()
final.close()
