class Node:
    def __init__(self, who, where, coordinate, pcDist):

        # who: The identifier indicating the owner (0 for driver, other # for passenger)
        # where: The identifier indicating the place (0 for origin, 1 for destination)
        # coordinate: (longitude, latitude)
        # pcDist: The distance between previous node and current node
        self.who = who
        self.where = where
        self.coordinate = coordinate
        self.pcDist = pcDist
    


    def gatherAttrs(self):
        return ", ".join("'{}': {}"
                        .format(k, getattr(self, k))
                        for k in self.__dict__.keys())

    def __str__(self):
        return "{{{}}}".format(self.gatherAttrs())


if __name__ == '__main__':
    Node()
