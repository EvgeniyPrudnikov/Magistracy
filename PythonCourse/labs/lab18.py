from bs4 import BeautifulSoup as sp


def find_links(filename):
    with open(filename) as f:
        h_doc = f.read()
        soup = sp(h_doc, 'html.parser')
        for link in soup.find_all('a'):
            print link.get('href')


find_links("Python-Wiki.html")
