import cv2 as cv
import numpy as np
import threading
import time
from keras.models import load_model


class Classifier(object):

    def __init__(self,
    VideoCaptureIndex=0,
    ColorModelFile='colorModel.hdf5',
    ObjectModelFile='Object.h5',
    DetectSquareLenth=40,
    CameraResolution=(480,640)):
        self.ColorModelFile = ColorModelFile
        self.ObjectModelFile = ObjectModelFile
        self.VideoCaptureIndex = VideoCaptureIndex
        self.RecW = DetectSquareLenth
        self.H, self.W = CameraResolution
        self.model = {'ColorModel':None,'ObjectModel':None}
        self.subthread = None
        self.cam = cv.VideoCapture(self.VideoCaptureIndex)
        self.cam.set(cv.CAP_PROP_FRAME_WIDTH,self.W)
        self.cam.set(cv.CAP_PROP_FRAME_HEIGHT,self.H)
        self.mode = 1
        self.value = {'Color':-1,'Object':-1,'Defect':-1}

    def judgeColor(self,model,roi):
        l=[]
        B,G,R = cv.split(roi)
        H,S,V = cv.split(cv.cvtColor(roi,cv.COLOR_BGR2HSV_FULL))
        Y,U,_V = cv.split(cv.cvtColor(roi,cv.COLOR_BGR2YUV))
        shapeH,shapeW = B.shape
        for _r in range(shapeH):
            for _c in range(shapeW):
                row = []
                for each in [H,S,V,Y,U,_V]:
                    row.append(each[_r,_c]/255)
                l.append(row)
        X = np.array(l)
        out = model.predict(X)
        out=np.mean(out,axis=0)
        return out.argmax()

    def judgeObject(self,model,roi):
        roi = cv.resize(roi,(40,40))
        cv.imshow('roi_snapshot',roi)
        roi = roi.reshape((1,)+roi.shape+(1,))
        result = model.predict_classes([roi])
        # print(result)
        return result[0]

    def getValue(self,mode):
        if mode == 0:
            return self.value['Object']
        elif mode == 1:
            return self.value['Color']
        elif mode == 2:
            return self.value['Defect']
        # return self.value

    def loop(self):
        LUX = (int)(self.W/2-self.RecW/2)
        LUY = (int)(self.H/2-self.RecW/2)
        RDX = (int)(self.W/2+self.RecW/2)
        RDY = (int)(self.H/2+self.RecW/2)
        self.model['ColorModel'] = load_model(self.ColorModelFile)
        self.model['ObjectModel'] = load_model(self.ObjectModelFile)
        while(True):
            time.sleep(0.01)
            suc,img = self.cam.read()
            if not suc:
                print('failed')
                break
            cc_img = img.copy()
            od_img = img.copy()
            dd_img = img.copy()
            cv.rectangle(cc_img,(LUX-1,LUY-1),(RDX,RDY),(0,255,0),1)
            cv.rectangle(od_img,(205-2,125-2),(435+1,355+1),(0,255,0),2)
            cv.rectangle(dd_img,(205-2,125-2),(435+1,355+1),(0,255,0),2)
            cc_roi = cc_img[LUY:RDY,LUX:RDX,:]
            cv.imshow('colorClassification',cc_img)
            od_roi = od_img[175:315,255:395]
            od_gray = cv.cvtColor(od_roi,cv.COLOR_BGR2GRAY)
            th,od_roi = cv.threshold(od_gray,80,255,cv.THRESH_BINARY)
            cv.imshow('objectClassification',od_img)
            # cv.imshow('roi_snapshot',od_roi)
            dd_roi = dd_img[185:305,265:385]
            dd_gray = cv.cvtColor(dd_roi,cv.COLOR_BGR2GRAY)
            th,dd_roi = cv.threshold(dd_gray,155,255,cv.THRESH_BINARY)
            cv.imshow('dd_snapshot',dd_roi)
            H,S,V = cv.split(cv.cvtColor(img,cv.COLOR_BGR2HSV))
            Y,U,V = cv.split(cv.cvtColor(img,cv.COLOR_BGR2YUV))
            self.value['Color'] = self.judgeColor(self.model['ColorModel'],cc_roi)
            self.value['Object'] = self.judgeObject(self.model['ObjectModel'],od_roi)
            self.value['Defect'] = 1 if np.min(dd_roi) == 0 else 0
            key = cv.waitKey(1)
            if key != -1:
                if key == 32:
                    cv.imwrite('color.jpg',img)
                    cv.destroyAllWindows()
                    break

    def Start(self):
        self.subthread = threading.Thread(target=self.loop)
        self.subthread.start()

    def IsAlive(self):
        return self.subthread.is_alive()
