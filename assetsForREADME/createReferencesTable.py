"""

How this python script works?

You pass an argument to it, where you specify path to your README.md text.
Then it looks at words (just splits initial text by spaces) and looks at 
all the links that occur in the text. So, your links should be separated with spaces.
Then it replaces with template that looks like so: 
[<sup>[ind-link-of-occurrence]</sup>](#reference-same-ind).
This creates an anchor to the element that is located in References section.
All links are stored in array and after text is processed, they are used to
create anchors at the bottom of the file.

How references section looks like(example)?
## 7. References

<a id="reference-1"></a>
1. [**^**](https://en.wikipedia.org/wiki/Ray_casting) Wikipedia raycasting page

<a id="reference-2"></a>
2. [**^**](https://proglib.io/p/raycasting-for-the-smallest) Article in Russian about raycasting

<a id="reference-3"></a>
3. [**^**](https://en.wikipedia.org/wiki/2.5D) What's a 2.5D, wiki page.

So, it's basically an enumeration of all used links in the README, with a proper name.

"""

import sys

RESULT_FILE_NAME = "READMEwithRefs.md"
linksDescriptions = [
    "Raycasting, wiki page",
    "Article about raycasting in Russian",
    "Wiki page about 2.5D",
    "Wiki page about FPS (first person shooter)",
    "Wolfenstein_3D",
    "Doom_1993",
    "FOV (field of view)",
    "Raycasting with fisheye effect correction GIF example, from wikipedia page",
    "SFML official page",
    "Point in polygon problem",
    "What's perspective, wiki page",
    "Inverse square law (for brightness of surface, depending on distance to it)",
    "Image to help understand fisheye effect, from proglib article",
    "Continuous function",
    "Habr article about raycasting",
    "Time complexity",
    "Scanline algorithm",
    "Parallel computing"
]

def getListOfWordsFromFile(fileName) -> list[str]:
    with open(fileName, "r") as file:
        words = file.read().split(' ')
        return words
    
def isWordAlink(word) -> bool:
    TMP = "https:"
    TMP_LEN = len(TMP)

    return len(word)      >= TMP_LEN and \
           word[:TMP_LEN] == TMP

def getResultTextAndListOfLinks(words) -> tuple[str, list[str]]:
    resultText = ""
    links      = []

    linkInd = 0
    for word in words:
        if not isWordAlink(word):
            resultText += word + " "
            continue

        linkInd += 1
        resultText += f"[<sup>[{linkInd}]</sup>](#reference-{linkInd})"
        links.append(word)

    return resultText, links

def addReferencesSection(resultText, links) -> str:
    assert(len(links) == len(linksDescriptions))

    resultText += "\n## References\n\n"
    for linkInd, link in enumerate(links, start=1):
        print(linkInd, link, linksDescriptions[linkInd-1])
        resultText += f"""
<a id="reference-{linkInd}"></a>
{linkInd}. [**^**]({link}) {linksDescriptions[linkInd-1]}."""

    return resultText

def saveResult2File(resultText) -> None:
    with open(RESULT_FILE_NAME, "w") as resultFile:
        resultFile.write(resultText)
    print("done...\n")

if __name__ == "__main__":
    if (len(sys.argv) != 2):
        print("This script takes only one argument: \n" \
        "path to the README, that is going to be processed")
        exit(1)

    path2README = sys.argv[1]
    print("path to README: ", path2README)
    words = getListOfWordsFromFile(path2README)

    resultText, links = getResultTextAndListOfLinks(words)

    resultText = addReferencesSection(resultText, links)
    saveResult2File(resultText)
