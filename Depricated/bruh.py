import json
import requests

def initData():
    pass

# Entry Point
if __name__ == "__main__":
    dataFile = open("data.json")
    jsonData = json.loads(dataFile.read())
        

    for i in range(jsonData["timeSteps"]):


