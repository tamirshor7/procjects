import string
from bs4 import BeautifulSoup
import requests as req
from Constants import *
from pathlib import Path
import pandas as pd
import sys
import xlsxwriter
import datetime
from tqdm import tqdm



def parseStatsChart(stock_stats_chart, stock_name, dest_filename):
    # - stock_stats_chart is a BeatifulSoup Object representing the part of the stock's html that contains the stats chart
    # desired stats is a list of stat identifiers to be taken, fitted to said HTML.
    # - stock_name is the name of currently parsed stock.
    # - dest_filename is the name of the excel file into which to information should be processed

    xl_obj = pd.ExcelFile(dest_filename, engine="openpyxl")
    writer = pd.ExcelWriter(path=file_name, mode='a', engine="openpyxl", if_sheet_exists="replace")
    sheets = xl_obj.sheet_names
    df_orig = pd.read_excel(dest_filename)
    if not stock_name in sheets:
        df_orig.to_excel(excel_writer=writer, index=False, sheet_name=stock_name)  # add sheet to workbook
        writer.save()

    df_orig = pd.read_excel(dest_filename, sheet_name=stock_name)
    df_new = pd.read_excel(dest_filename, sheet_name=stock_name)
    field = stock_stats_chart.find_all("td")
    name = ""
    timestamp = datetime.datetime.now().strftime("%x") + "\n " + datetime.datetime.now().strftime("%X")
    if TIMESTAMP_COL_NAME in df_new.head():  # Need to treat differently if header already exists or not
        df_new["Timestamp"][0] = timestamp
    else:
        df_new["Timestamp"] = [timestamp]

    i = 1  # i mod 2 indicates whether current token is stock name (1) or value (0). Value to skip starter lines from html
    for token in field:
        if i % 2 == 0:
            # df_new[name] = [token.text]
            if name in df_new.head():  # Need to treat differently if header already exists or not
                df_new[name][0] = token.text
            else:
                df_new[name] = [token.text]

        else:
            name = token.text

        i += 1

    df_orig = df_orig.append(df_new)
    df_orig = df_orig.sort_values(by= ["Timestamp"])

    df_orig.to_excel(excel_writer=writer, index=False, sheet_name=stock_name, startcol=0, startrow=0)
    for letter in string.ascii_uppercase:  # Adjust columns' widths
        writer.sheets[stock_name].column_dimensions[letter].width = COL_WIDTH

    writer.save()


def getExcelFilename():
    #Retrieve destination filename. Create file if inexistent
    dest_file_name = DEFAULT_DEST_CSV_NAME
    if len(sys.argv) > 1:
        dest_file_name = sys.argv[1]

    # create file if inexistent
    if not Path(dest_file_name).is_file():
        workbook = xlsxwriter.Workbook(dest_file_name)
        workbook.close()
    return dest_file_name


try:
    file_name = getExcelFilename()
    source = req.get(SOURCE_URL, headers=HEADERS).text
    obj = BeautifulSoup(source, "lxml")

    pd.options.mode.chained_assignment = None  # default='warn'

    # dest_file = open("scanned.csv")
    stock_zone = obj.find("body")
    stock_links = stock_zone.find_all("a", class_="tab-link")
    filtered = list(stock_links)  # filter recurring stocks
    filtered = list(dict.fromkeys(filtered))
    count = 0



    #Go over data, extract each stock and send to parsing
    for link, count in zip(filtered, tqdm(range(min(len(filtered), DEFAULT_MAX_TO_SCAN)))):
        url_query = URL_QUERY_PREFIX + link.text
        stock_page = req.get("https://finviz.com/" + url_query, headers=HEADERS).text
        stock_page_obj = BeautifulSoup(stock_page, "lxml")
        stats_chart = stock_page_obj.find("table", class_=HTML_STATS_CHART_IDENTIFIER)
        if stats_chart is None:
            continue
        parseStatsChart(stats_chart, link.text, file_name)
        count += 1



except req.exceptions.ConnectionError:
    print("ERROR: Could Not Connect to Host. Please Verify Internet Connection and Host Availability")

except PermissionError:
    print("ERROR: Target file seems to be open. Please close it and rerun to continue")
except:
    print("ERROR: Unknown Error Occurred.")


