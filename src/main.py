import json
import requests
import matplotlib.pyplot as plt
import numpy as np
import os
import cv2
import imageio
import PIL
import io
import sys

# total arguments
n = len(sys.argv)

# filename = 'simTrial0.json'
filename = 'optimizationTrialData.json'


testingFolder = 'testing'
if (n >= 2):
    testingFolder = sys.argv[1]

pathW = '\\data\\'+testingFolder+'\\'
pathL = '/data/'+testingFolder+'/'
path = pathW
if (n >= 1):
    # if(sys.argv[0] == 'w'):
    if (True):
        path = pathW
    else:
        path = pathL
fileName = path + filename


def saveToMp4():
    with imageio.get_writer(os.getcwd()+path+'output'+testingFolder+'.gif', mode='I', fps=3) as writer:
        for image in [img for img in os.listdir(os.getcwd()+path) if img.endswith(".png")]:
            # for image in imagesStuff:

            processedImage = imageio.imread(os.getcwd()+path+image)
            writer.append_data(processedImage)

    # images = [img for img in os.listdir(path) if img.endswith(".png")]
    # frame = cv2.imread(os.path.join(path, images[0]))
    # height, width, layers = frame.shape

    # video = cv2.VideoWriter(video_name, 0, 1, (width,height))

    # for image in images:
    #     video.write(cv2.imread(os.path.join(path, image)))

    # cv2.destroyAllWindows()
    # video.release()


def saveImages():
    with open(os.getcwd()+fileName, 'r') as file:
        data = json.load(file, strict=False)
        nr = data['x']
        nc = data['y']
        weights = data["distributionMap"]
        figureArr = []
        print(len(weights))
        for i in range(120, len(weights), 10):
            # Convert the comma-separated string to a 2D numpy array
            # weights_array = np..reshape(nr, nc)
            weights_array = np.matrix(weights[i])

            plt.axes().set_aspect(2)
            plt.xlim(0, nc + 1)
            plt.ylim(0, nr + 1)
            plt.axis('off')  # Set the aspect ratio outside the loop

            for row in range(1, nr + 1):
                for col in range(1, nc + 1):
                    # Use the weight value as the color intensity
                    weight_value = weights_array[row - 1, col - 1]/10
                    color = plt.cm.RdYlGn(weight_value)
                    plt.plot(col, row, marker='s', markersize=4.5, color=color)

            plt.axis('off')
            # figureArr.append(plt)
            plt.savefig(os.getcwd() + path + 'simImage' + str(i) + '.png')

            # canvas = FigureCanvasAgg(plt)
            # canvas.draw()
            # width, height = canvas.get_width_height()
            # image_array = np.array(canvas.buffer_rgba())
            # image_array = image_array.reshape(height, width, 3)

            # imagesStuff.append(np.frombuffer(image_array, dtype=np.uint8))
            # plt.show()
            # buf = io.BytesIO()
            # image = plt.savefig(buf, format = 'png')
            # imagesStuff.append(image)
            # print('appended')
        print(len(figureArr))
# saveImages()
# saveToMp4()


def generateSingleGraph(array, filename, i):
    for coord_list in array:
        # print(len(coord_list))
        x_coords, y_coords = zip(*coord_list)
        plt.plot(x_coords, y_coords, 'o-', linestyle='dashed')
    listSum = array[0]
    for i in range(2):
        for j in range(1, len(array)):
            for k in range(len(array[0])):
                listSum[k][i] += array[j][k][i]
    for x in range(len(listSum)):
        for y in range(2):
            listSum[x][y] = listSum[x][y]/10

    x_coords, y_coords = zip(*listSum)
    plt.plot(x_coords, y_coords, color='k', linestyle='solid', linewidth=3)

    # ylabelName = ''
    plt.xlabel('Harvest Day')
    plt.ylabel('Cost Function')

    plt.savefig(filename)
    plt.cla()


def generateGraph():
    with open(os.getcwd()+fileName, 'r') as file:
        data = json.load(file, strict=False)
        outputVec = data["outputVec"]
        for i in range(len(outputVec)):
            # print(len(outputVec[i]))
            # print(i)
            generateSingleGraph(outputVec[i], 'graph' + str(i) + '.png', i)

generateGraph()


def generateSingleGraph2(array, filename):

    for i in range(len(array[0])):
        array[0][i] = array[0][i]/20
        array[1][i] = array[1][i]/20

    miniArr = []
    for i in range(len(array[0])):
        miniArr.append(i)

    plt.plot(miniArr, array[0], 'o-', linestyle='dashed', label='femaleEgg')
    plt.plot(miniArr, array[1], 'o-', linestyle='dashed', label='maleEgg')
    plt.plot(miniArr, array[2], 'o-', linestyle='dashed', label='femaleNymph')
    plt.plot(miniArr, array[3], 'o-', linestyle='dashed', label='maleNymph')
    plt.plot(miniArr, array[4], 'o-', linestyle='dashed', label='femaleAdult')
    plt.plot(miniArr, array[5], 'o-', linestyle='dashed', label='maleAdult')
    plt.legend(loc='upper right')
    # for i in range(len(array)):
    # print(len(coord_list))
    # plt.plot(miniArr, array[i], 'o-', linestyle='dashed')
    # listSum = array[0]
    # for i in range(6):
    #     for j in range(1, len(array)):
    #         for k in range(len(array[0])):
    #             listSum[k][i] += array[j][k][i]
    # for x in range(len(listSum)):
    #     for y in range(2):
    #         listSum[x][y] = listSum[x][y]/6

    # x_coords, y_coords = zip(*listSum)
    # plt.plot(x_coords, y_coords, color='k',linestyle='solid', linewidth=3)

    # ylabelName = ''
    plt.xlabel('Bug Population')
    plt.ylabel('Time Step')

    plt.savefig(filename)
    plt.cla()


def generateGraph2(fName, o):
    with open(os.getcwd()+path+fName, 'r') as file:
        data = json.load(file, strict=False)
        outputVec = data["sumBugs"]

        newVec = []

        for k in range(6):
            sumVec = []
            for i in range(len(outputVec)):
                sumVec.append(outputVec[i][k])
            newVec.append(sumVec)

        # for i in range(len(newVec)):
            # print(len(outputVec[i]))
            # print(i)
        generateSingleGraph2(newVec, 'graph' + str(o) + '.png')


# for i in range(10):
    # generateGraph2('simTrial'+str(i)+'.json', i)
