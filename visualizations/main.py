import json
import requests

def initData():
    pass

filename = "output.json"

# Entry Point
if __name__ == "__main__":
    dataFile = open(filename)
    jsonData = json.loads(dataFile.read())
    csv = jsonData["weightMap"]
    x = jsonData["x"]
    y = jsonData["y"]

    


