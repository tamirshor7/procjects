Python Stock Scanner :
    The following script scrapes stats regarding a given amount of stocks from a stock analysis website ('finviz.com'), and outputs all collected data to a given
    spreadsheet (created if inexistent, appended otherwise). maximum number of stocks to be scraped can be modified either by CLI activation input, or by changing the
    designated DEFAULT_MAX_TO_SCAN variable in the Constants.py file. Sheet name can also be varied through there.
    To run script run Scanner.py.

    The script is accompanied by a VBA script that collects all data in the activated sheet and avregizes each stat for each stock, over all existent data.
    You will need to create a VBA module (in the Excel toolbar dev mode) and insert the code and joined VBA Globals file there to run the script.
