#!/usr/bin/env python3
import sys
import re
import itertools

streetDict = {}
intersectionList = []
nodeList = []
nodeDict = {}
edgeList = []


class Street:

    def __init__(self, name, edges):
        self.edges = edges
        self.name = name

    def addEdge(self, newEdge):
        if (newEdge.node1.coord == newEdge.node2.coord):
            return False

        for edge in self.edges:
            if (edge.node1.coord == newEdge.node1.coord and edge.node2.coord == newEdge.node2.coord):
                return False

        self.edges.append(newEdge)


class Edge:
    def __init__(self, node1, node2, streetName):
        self.node1 = node1
        self.node2 = node2
        self.street = streetName


class Node:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.coord = [round(float(x), 4), round(float(y), 4)]


def parseInput(line):
    lineinput = line.strip().split('"')
    return lineinput


def addStreet(textInput):
    rawData = textInput[2].strip().replace(" ", "")
    rawNodes = re.findall(r"[-]?\d+", rawData)

    nodes = []

    for i in range(0, len(rawNodes), 2):
        isVisited = False
        node = Node(rawNodes[i], rawNodes[i + 1])

        for existedNode in nodeList:

            # print(existedNode.coord,"=",node.coord)
            if existedNode.coord == node.coord:
                isVisited = True
                nodes.append(existedNode)
                break

        if (not isVisited):
            nodes.append(node)
            nodeList.append(node)

    edges = []
    for i in range(1, len(nodes)):
        edges.append(Edge(nodes[i - 1], nodes[i], textInput[1].lower()))

    # for node in nodes:
    #     print(node.coord)

    # for edge in edges:
    #     print(edge.node1.x,edge.node1.y,edge.node2.x,edge.node2.y)

    streetDict[textInput[1].lower()] = (Street(textInput[1].lower(), edges))


def changeStreet(textInput):
    removeStreet(textInput)
    addStreet(textInput)


def removeStreet(textInput):
    streetName = textInput[1].lower()

    streetDict.pop(streetName)

    for edge in edgeList.copy():
        if edge.street == streetName:

            edgeList.remove(edge)
            # print(edge.node1.coord,edge.node2.coord)
            if (edge.node1 in nodeList):
                nodeList.remove(edge.node1)
            if (edge.node2 in nodeList):
                nodeList.remove(edge.node2)

    for intersection in intersectionList.copy():
        # print(intersection.coord)
        count = 0
        edgesConnected = []
        for edge in edgeList:
            if (edge.node1.coord == intersection.coord or edge.node2.coord == intersection.coord):
                count = count + 1
                edgesConnected.append(edge)
        # print(count)
        if count == 2:
            intersectionList.remove(intersection)
            edgeList.remove(edgesConnected[0])
            edgeList.remove(edgesConnected[1])
            streetDict[edgesConnected[0].street].edges.remove(edgesConnected[0])
            streetDict[edgesConnected[0].street].edges.remove(edgesConnected[1])

            if (edgesConnected[0].node1 is intersection):
                if (edgesConnected[1].node1 is intersection):
                    newEdge = Edge(edgesConnected[0].node2, edgesConnected[1].node2, edgesConnected[0].street)
                else:
                    newEdge = Edge(edgesConnected[0].node2, edgesConnected[1].node1, edgesConnected[0].street)
            else:
                if (edgesConnected[1].node1 is intersection):
                    newEdge = Edge(edgesConnected[0].node1, edgesConnected[1].node2, edgesConnected[0].street)
                else:
                    newEdge = Edge(edgesConnected[0].node1, edgesConnected[1].node1, edgesConnected[0].street)

            edgeList.append(newEdge)
            streetDict[edgesConnected[0].street].edges.append(newEdge)

    # for edge in edgeList:
    #     print(edge.node1.x,edge.node1.y,edge.node2.x,edge.node2.y)

    # for node in nodeList:
    #     print(node.coord)


def updateGraph(streetList):
    # for street in streetList:
    #     for edge in street.edges:
    #         print(edge.node1.x,edge.node1.y,edge.node2.x,edge.node2.y)

    foundIntersection = False
    for i in range(0, len(streetList)):
        if i + 1 < len(streetList):
            for j in range(i + 1, len(streetList)):
                streeti = streetList[i]
                streetj = streetList[j]

                for index0 in range(len(streeti.edges)):
                    for index1 in range(len(streetj.edges)):
                        edge0 = streeti.edges[index0]
                        edge1 = streetj.edges[index1]
                        # print(edge0.node1.coord,edge0.node2.coord,edge1.node1.coord,edge1.node2.coord)
                        intersection = calulate_cross_lines(edge0.node1.coord, edge0.node2.coord, edge1.node1.coord,
                                                            edge1.node2.coord)
                        if intersection == "None":
                            continue
                        else:
                            # print("intersec:")
                            # print(edge0.node1.coord,edge0.node2.coord,edge1.node1.coord,edge1.node2.coord)
                            # print(intersection)
                            # print("end")
                            intersectionNode = Node(round(float(intersection[0]), 4), round(float(intersection[1]), 4))
                            foundIntersection = True
                            for node in intersectionList:
                                # print(node.coord,"=",intersectionNode.coord)
                                if (intersectionNode.coord == node.coord):
                                    intersectionNode = node
                                    foundIntersection = False
                                    # print("repeated")
                                    break

                            if edge0 in streeti.edges:
                                streeti.edges.remove(edge0)
                            if edge1 in streetj.edges:
                                streetj.edges.remove(edge1)
                            streeti.addEdge(Edge(edge0.node1, intersectionNode, edge0.street))
                            streeti.addEdge(Edge(edge0.node2, intersectionNode, edge0.street))
                            streetj.addEdge(Edge(edge1.node1, intersectionNode, edge1.street))
                            streetj.addEdge(Edge(edge1.node2, intersectionNode, edge1.street))
                            if foundIntersection:
                                intersectionList.append(intersectionNode)
                            break
                    if foundIntersection:
                        break

    if foundIntersection:
        return False

    return True


def calulate_cross_lines(line0_pos0, line0_pos1, line1_pos0, line1_pos1):
    line0_a = line0_pos0[1] - line0_pos1[1]
    line0_b = line0_pos1[0] - line0_pos0[0]
    line0_c = line0_pos0[0] * line0_pos1[1] - line0_pos1[0] * line0_pos0[1]
    line1_a = line1_pos0[1] - line1_pos1[1]
    line1_b = line1_pos1[0] - line1_pos0[0]
    line1_c = line1_pos0[0] * line1_pos1[1] - line1_pos1[0] * line1_pos0[1]
    d = line0_a * line1_b - line1_a * line0_b
    if d == 0:
        return "None"

    x = (line0_b * line1_c - line1_b * line0_c) * 1.0 / d
    y = (line0_c * line1_a - line1_c * line0_a) * 1.0 / d
    pt = [round(x, 4), round(y, 4)]

    if round((x - line0_pos0[0]), 4) * round((x - line0_pos1[0]), 4) <= 0 and round((x - line1_pos0[0]), 4) * round(
            (x - line1_pos1[0]), 4) <= 0 and round((y - line0_pos0[1]), 4) * round((y - line0_pos1[1]),
                                                                                   4) <= 0 and round(
            (y - line1_pos0[1]), 4) * round((y - line1_pos1[1]), 4) <= 0:
        count = 0
        if (pt == line0_pos0): count = count + 1
        if (pt == line0_pos1): count = count + 1
        if (pt == line1_pos0): count = count + 1
        if (pt == line1_pos1): count = count + 1
        if (count < 2):
            return pt
        else:
            return "None"


    else:
        return "None"


def validation(line):
    if line == "":
        return 0
    elif re.match(
            "\s*[ac]\s+[\"][a-zA-Z ]*[\"]\s+[(]\s*-?\s*[0-9 ]+\s*[,]\s*-?\s*[0-9 ]+\s*[)](\s*[(]\s*-?\s*[0-9 ]+\s*[,]\s*-?\s*[0-9 ]+\s*[)]\s*)+",
            line):
        return 1
    elif re.match("\s*[r]\s+[\"][a-zA-Z ]*[\"]\s+", line):
        return 1
    elif re.match("\s*[g]\s*", line):
        return 1
    else:
        return 2


def main():
    while True:
        try:
            line = sys.stdin.readline()
            x = validation(line)
            if x == 0:
                break
            elif x == 2:
                print("Error:Input format is invalid")
            elif x == 1:

                textInput = parseInput(line)
                cmd = textInput[0].replace(" ", "")
                # TODO - add input parse
                if cmd == "a":
                    addStreet(textInput)
                elif cmd == "c":
                    changeStreet(textInput)
                elif cmd == "r":
                    removeStreet(textInput)
                elif cmd == "g":
                    while (not updateGraph(list(streetDict.values()))):
                        continue

                    # for i in intersectionList:
                    #     print(i.coord)

                    for street in list(streetDict.values()):
                        for edge in street.edges:
                            if not (edge in edgeList):
                                edgeList.append(edge)

                    # for edge in edgeList:
                    #     print(edge.node1.x,edge.node1.y,edge.node2.x,edge.node2.y).

                    deletedNode = []
                    filteredEdge = []

                    for edge in edgeList:
                        if (edge.node1 in intersectionList) or (edge.node2 in intersectionList):
                            if (nodeDict.get(edge.node1) == None):
                                nodeDict[edge.node1] = len(nodeDict) + 1
                            if (nodeDict.get(edge.node2) == None):
                                nodeDict[edge.node2] = len(nodeDict) + 1
                            filteredEdge.append(edge)

                    for node in nodeDict.keys():
                        valid = False
                        if not node in nodeList:
                            valid = False

                        for edge in filteredEdge:
                            if node.coord == edge.node1.coord or node.coord == edge.node2.coord:
                                valid = True

                        if (not valid):
                            deletedNode.append(node)

                    for node in deletedNode:
                        nodeDict.pop(node)

                    print("V {}".format(len(nodeDict)), flush=True)

                    edges = []
                    temp = []
                    for edge in edgeList:
                        if (edge.node1 in intersectionList) or (edge.node2 in intersectionList):
                            edges.append([nodeDict[edge.node1], nodeDict[edge.node2]])
                            temp.append(nodeDict[edge.node1])
                            temp.append(nodeDict[edge.node2])

                    if len(edges) == 0:
                        print("E {}", flush=True)
                    else:
                        offset = min(temp)
                        print("E {", end="", flush=True)
                        for i in range(len(edges) - 1):
                            print("<{},{}>,".format(edges[i][0] - offset,edges[i][1] - offset), end="", flush=True)
                        print("<{},{}>".format(edges[-1][0] - offset, edges[-1][1] - offset) + "}", flush=True)
                    # break
        except KeyError:
            print(r"Error:'c' or 'r' specified for a street that does not exist.")

    sys.exit(0)


if __name__ == "__main__":
    main()






