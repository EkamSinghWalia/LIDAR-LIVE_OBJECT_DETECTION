import cv2
import numpy as np 

thres = 0.45 #threshold to detetct objects
nms_threshold = 0.2

#cap =cv2.imread('photo.jpg')
cap = cv2.VideoCapture(0)
#cap.set(3,648)#3 means width
#cap.set(4,488)#4 means height 
# cap.set(10,150)# for brightness

classNames = []
classFile = 'coco.names'
with open(classFile,'rt') as f: #rt is read
    classNames = f.read().rstrip('\n').split('\n')#\n is next line 
#print(classNames)



configPath='ssd_mobilenet_v3_large_coco_2020_01_14.pbtxt'
weightsPath='frozen_inference_graph.pb'

net = cv2.dnn_DetectionModel(weightsPath,configPath)
net.setInputSize(320,320)
net.setInputScale(1.0/ 127.5)
net.setInputMean((127.5, 127.5, 127.5))
net.setInputSwapRB(True)

while True:
    success,img = cap.read()
    classIds, confs, bbox = net.detect(img,confThreshold=thres)#bbox is for the rectangle and classsidds is for the object name ,threshold is given for 50% sureity that the object in front is actually an object
    #print(classIds,bbox)
    bbox = list(bbox)
    confs = list(np.array(confs).reshape(1,-1)[0])#converting numpy array to a list ,[0] can be used to remove a pair of square backets 
    confs = list(map(float,confs))
    #print(type(confs[0]))
    #print(confs)


    
    indices = cv2.dnn.NMSBoxes(bbox,confs,thres,nms_threshold)
    #print(indeces)



    for i in indices:
        i = i[0]
        box = bbox[i]
        x,y,w,h = box[0],box[1],box[2],box[3]
        cv2.rectangle(img,(x,y),(x+w,h+y), color=(0,255,0),thickness=2)
        cv2.putText(img,classNames[classIds[i][0]-1].upper(),(box[0]+10,box[1]+30),
                        cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)
        
##    if len(classIds) != 0:
##        for classId,confidence,box in zip (classIds.flatten(),confs.flatten(),bbox):#we use zip i for loop for iterating more than one item in a single iteration 
##            cv2.rectangle(img,box,color=(0,255,0),thickness=2)
##            cv2.putText(img,classNames[classId-1].upper(),(box[0]+10,box[1]+30),
##                        cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)
##            cv2.putText(img,str(round(confidence*100,2)),(box[0]+200,box[1]+30),
##                        cv2.FONT_HERSHEY_COMPLEX,1,(0,255,0),2)


    cv2.imshow("Output",img)
    cv2.waitKey(1)

