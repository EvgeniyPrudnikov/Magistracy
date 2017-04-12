import scrapy
from wiki_demo.items import WikiDemoItem

import re

class FirstLinkSpider(scrapy.Spider):
    name = 'first_link'
    start_urls = ['https://en.wikipedia.org/wiki/Minsk']
    
    visited_urls = set()
    header_selector = 'h1#firstHeading.firstHeading::text'
    body_link_selector = '(//div[@id="mw-content-text"]/p/a/@href)[position() < 100]'
    allowed_re = re.compile('https://.+.wikipedia\.org/wiki/'
                            '(?!((File|Talk|Category|Portal|Special|'
                            'Template|Template_talk|Wikipedia|Help|Draft):|Main_Page)).+')
       
    def parse(self, response):
	item = WikiDemoItem()        
	item['title'] = response.css(self.header_selector).extract_first()
        item['url'] = response.url
	yield item
        self.visited_urls.add(response.url)
        if response.url == 'https://en.wikipedia.org/wiki/Philosophy':
            raise StopIteration

        for link in response.xpath(self.body_link_selector).extract():
            if link[0] == '#':
                continue
            next_url = response.urljoin(link)
            if self.allowed_re.match(next_url) and not next_url in self.visited_urls:
                yield scrapy.Request(next_url, callback=self.parse)
                break
