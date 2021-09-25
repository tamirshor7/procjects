SOURCE_URL = "https://finviz.com/"
URL_QUERY_PREFIX = "/quote.ashx?t="
HTML_STATS_CHART_IDENTIFIER = "snapshot-table2"
DEFAULT_DEST_CSV_NAME = "Stocks_th.xlsx"
HEADERS = {'User-Agent': 'My User Agent 1.0', 'From': 'youremail@domain.com'} #dummy header to bypass host protection
COL_WIDTH = 17
TIMESTAMP_COL_NAME = "Timestamp"
DEFAULT_MAX_TO_SCAN = 10
THREAD_NUM = 4


#Stat Identifiers (finviz)
PROFIT_TO_EARNINGS = "P/E"
FORECASTED_PROFIT_TO_EARNINGS = "Forward P/E"
EARNINGS_PER_SHARE = "EPS (ttm)"
TRADED_SHARES_NUM = "Shs Outstand"
WEEKLY_PERFORMANCE = "Perf Week"
MONTHLY_PERFORMANCE = "Perf Month"
QUARTERLY_PERFORMANCE = "Perf Quarter"
HALF_YEAR_PERFORMANCE = "Performance (Half Year)"
MARKET_CAPITAL = "Market Cap"
FORCASTED_EPS = "EPS next Y"
INCOME = "Income"
PE_TO_EPS = "PEG"


DEFAULT_STAT_PACKAGE = [PROFIT_TO_EARNINGS,FORECASTED_PROFIT_TO_EARNINGS,EARNINGS_PER_SHARE, MARKET_CAPITAL,
                        WEEKLY_PERFORMANCE, MONTHLY_PERFORMANCE, QUARTERLY_PERFORMANCE, HALF_YEAR_PERFORMANCE]

'Error classes'
class ThreadNumError(Exception):
    pass