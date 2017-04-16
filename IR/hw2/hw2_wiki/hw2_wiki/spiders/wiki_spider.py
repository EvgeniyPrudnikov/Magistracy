# -*- coding: utf-8 -*-

import scrapy
from hw2_wiki.items import WikiItem
from bs4 import BeautifulSoup
import re


class WikiSpider(scrapy.Spider):
    name = 'wiki_spider'
    start_urls = ['https://en.wikipedia.org/wiki/Minsk',
                  'https://en.wikipedia.org/wiki/Australia',
                  'https://en.wikipedia.org/wiki/United_States',
                  'https://en.wikipedia.org/wiki/Russia']

    visited_urls = set()
    header_selector = 'h1#firstHeading.firstHeading::text'
    body_link_selector = '(//div[@id="mw-content-text"]/p/a/@href)[position() < 100]'

    allowed_re = re.compile('https://.+.wikipedia\.org/wiki/'
                            '(?!((File|Talk|Category|Portal|Special|'
                            'Template|Template_talk|Wikipedia|Help|Draft):|Main_Page)).+')

    def parse(self, response):

        item = WikiItem()
        item['title'] = response.css(self.header_selector).extract_first()
        item['url'] = response.url

        item['snippet'] = BeautifulSoup(response.xpath('//div[ @ id = "mw-content-text"] / p[1]').extract_first(),
                                        'lxml').get_text()[:255].encode('utf-8') + '...'

        item['linked_urls'] = response.request.headers.get('referer', None)

        yield item

        self.visited_urls.add(response.url)
        linked_links = [response.urljoin(link) for link in response.xpath(self.body_link_selector).extract() if
                        link[0] != '#']

        for next_url in linked_links:
            if self.allowed_re.match(next_url) and next_url not in self.visited_urls:
                yield scrapy.Request(next_url, callback=self.parse, headers={'referer': response.url})