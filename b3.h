#ifndef B3_HEADER_INCLUDED
#define B3_HEADER_INCLUDED

/*
 * b3.h - Header-only library for parsing B3 FIX/SBE messages.
 */

/*
 * Copyright (C) 2023  Gabriel de Brito
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>

/*
 * Structs on the protocol are so well-designed that probably
 * __attribute__((packed)) is not necessary and only serves to unable the
 * compilation on other compilers than gcc. Probably nobody will try to compile
 * this on idk tcc anyways so whatever. C should have a proper way to ensure
 * struct layout by the way.
 *
 * Enums must have a very specific size so we can't use C enums, and that
 * fucking sucks. This language should have a proper way to ensure an enum is of
 * a specific size, but what to expect when you can't ensure a struct have the
 * alignment you specify without a GNU extension? Because of that, our enums are
 * actually uint8_t with a sequence of #defines after.
 *
 * - "Naaaahhh! But you can just -fshort-enums!"
 *
 * Ahhh of course let's rely on a fucking flag passed to the compiler ensuring a
 * size that may change from machine to machine. We could even
 * __attribute__((packed)) like with structs but remember your dumb dumba we
 * need to ensure a size and not ensure smalleness.
 *
 * OH, AND BY THE WAY I didn't talked about the endianess. It's fucking dumb. I
 * need to like put the compiler in this "other mode" with pragma like if I was
 * programming in fucking Lisp and them reset it and hope it works. Like instead
 * of using a uint8le type or something and just using little-endian for the
 * struct order (as it would make no difference at all), I have to use other
 * weird extension instead of plain C. And wait, is there a motherfucker out
 * there using big endian?
 */

#pragma scalar_storage_order little-endian

typedef uint16_t B3TemplateID;
#define B3T_SEQUENCE_RESET 1
#define B3T_SEQUENCE 2
#define B3T_SECURITY_STATUS 3
#define B3T_NEWS 5
#define B3T_EMPTY_BOOK 9
#define B3T_SECURITY_GROUP_PHASE 10
#define B3T_CHANNEL_RESET 11
#define B3T_SECURITY_DEFINITION 12
#define B3T_OPENING_PRICE 15
#define B3T_THEORETICAL_OPENING_PRICE 16
#define B3T_CLOSING_PRICE 17
#define B3T_AUCTION_IMBALANCE 19
#define B3T_PRICE_BAND 22
#define B3T_QUANTITY_BAND 21
#define B3T_HIGH_PRICE 24
#define B3T_LOW_PRICE 25
#define B3T_LAST_TRADE_PRICE 27
#define B3T_ORDER_MBO 50
#define B3T_DELETE_ORDER_MBO 51
#define B3T_MASS_DELETE_ORDERS_MBO 52
#define B3T_TRADE 53
#define B3T_FORWARD_TRADE 54
#define B3T_EXECUTION_SUMMARY 55
#define B3T_EXECUTION_STATISTICS 56
#define B3T_TRADE_BUST 57
#define B3T_SNAPSHOT_FULL_REFRESH_HEADER 30
#define B3T_SNAPSHOT_FULL_REFRESH_ORDERS_MBO 71

/*
 * By the way you're literally supposed to make something like this:
 *
 * uint8_t *buf = receive_packet_from_somewhere();
 * B3PacketHeader *p_header = (B3PacketHeader*) buf;
 * B3MessageHeader *m_header = (B3MessageHeader*) &buf[16];
 *
 * And so forth for the rest of the messages...
 */

typedef struct {
	uint8_t channel_id;
	uint8_t reserved;
	uint16_t sequence_version;
	uint32_t sequence_number;
	uint64_t sending_time;
} __attribute__((packed)) B3PacketHeader;

typedef struct {
	uint16_t length;
	/* Encoding shall be always 0x50eb for use in B3. */
	uint16_t encoding;
	uint16_t block_length;
	B3TemplateID template_id;
	uint16_t schema_id;
	uint16_t schema_version;
} __attribute__((packed)) B3MessageHeader;

/*
 * Sets.
 */

typedef uint8_t B3MatchEventIndicator;
#define B3_MATCH_EVENT_LAST_TRADE_MSG (1 << 0)
#define B3_MATCH_EVENT_LAST_VOLUME_MSG (1 << 1)
#define B3_MATCH_EVENT_LAST_QUOTE_MSG (1 << 2)
#define B3_MATCH_EVENT_LAST_STATS_MSG (1 << 3)
#define B3_MATCH_EVENT_LAST_IMPLIED_MSG (1 << 4)
#define B3_MATCH_EVENT_RECOVERY_MSG (1 << 5)
/* Bit 7 is reserved for future use. */
#define B3_MATCH_EVENT_END_OF_EVENT (1 << 7)

/* I don't know why this is a 16 bit number but it's what it it's. */
typedef uint16_t B3ImbalanceCondition;
#define B3_IMBALANCE_CONDITION_MORE_BUYERS (1 << 8)
#define B3_IMBALANCE_CONDITION_MORE_SELLERS (1 << 9)

typedef uint16_t B3TradeCondition;
#define B3_TRADE_CONDITION_OPENING_PRICE (1 << 0)
#define B3_TRADE_CONDITION_CROSSED (1 << 1)
#define B3_TRADE_CONDITION_LAST_TRADE_AT_SAME_PRICE (1 << 2)
#define B3_TRADE_CONDITION_OUT_OF_SEQUENCE (1 << 3)
#define B3_TRADE_CONDITION_TRADE_ON_BEHALF (1 << 6)
#define B3_TRADE_CONDITION_REGULAR_TRADE (1 << 13)
#define B3_TRADE_CONDITION_BLOCK_TRADE (1 << 14)

/*
 * Enums.
 */

typedef uint8_t B3SecurityIDSource;
#define B3_SECURITY_ID_SOURCE_ISIN ((uint8_t) '4')
#define B3_SECURITY_ID_SOURCE_EXCHANGE_SYMBOL ((uint8_t) '8')

typedef uint8_t B3SecurityUpdateAction;
#define B3_SECURITY_UPDATE_ADD ((uint8_t) 'A')
#define B3_SECURITY_UPDATE_DELETE ((uint8_t) 'D')
#define B3_SECURITY_UPDATE_MODIFY ((uint8_t) 'M')

typedef uint8_t B3MDEntryType;
#define B3_MD_ENTRY_BID ((uint8_t) '0')
#define B3_MD_ENTRY_OFFER ((uint8_t) '1')
#define B3_MD_ENTRY_TRADE ((uint8_t) '2')
#define B3_MD_ENTRY_INDEX_VALUE ((uint8_t) '3')
#define B3_MD_ENTRY_OPENING_PRICE ((uint8_t) '4')
#define B3_MD_ENTRY_CLOSING_PRICE ((uint8_t) '5')
#define B3_MD_ENTRY_SETTLEMENT_PRICE ((uint8_t) '6')
#define B3_MD_ENTRY_SESSION_HIGH_PRICE ((uint8_t) '7')
#define B3_MD_ENTRY_SESSION_LOW_PRICE ((uint8_t) '8')
#define B3_MD_ENTRY_EXECUTION_STATISTICS ((uint8_t) '9')
#define B3_MD_ENTRY_IMBALANCE ((uint8_t) 'A')
#define B3_MD_ENTRY_TRADE_VOLUME ((uint8_t) 'B')
#define B3_MD_ENTRY_OPEN_INTEREST ((uint8_t) 'C')
#define B3_MD_ENTRY_EMPTY_BOOK ((uint8_t) 'J')
#define B3_MD_ENTRY_SECURITY_TRADING_STATE_PHASE ((uint8_t) 'c')
#define B3_MD_ENTRY_PRICE_BAND ((uint8_t) 'g')
#define B3_MD_ENTRY_QUANTITY_BAND ((uint8_t) 'h')
#define B3_MD_ENTRY_COMPOSITE_UNDERLYING_PRICE ((uint8_t) 'D')
#define B3_MD_ENTRY_EXECUTION_SUMMARY ((uint8_t) 's')
#define B3_MD_ENTRY_VOLATILITY_PRICE ((uint8_t) 'v')
#define B3_MD_ENTRY_TRADE_BUST ((uint8_t) 'u')

typedef uint8_t B3TradingSessionID;
#define B3_TRADING_SESSION_ID_REGULAR 1
#define B3_TRADING_SESSION_ID_NON_REGULAR 6

typedef uint8_t B3SecurityTradingStatus;
#define B3_SECURITY_TRADING_STATUS_PAUSE 2
#define B3_SECURITY_TRADING_STATUS_CLOSE 4
#define B3_SECURITY_TRADING_STATUS_OPEN 17
#define B3_SECURITY_TRADING_STATUS_FORBIDDEN 18
#define B3_SECURITY_TRADING_STATUS_UNKNOWN 20
#define B3_SECURITY_TRADING_STATUS_RESERVED 21
#define B3_SECURITY_TRADING_STATUS_CLOSING_CALL 101

typedef uint8_t B3SecurityTradingEvent;
#define B3_SECURITY_TRADING_EVENT_SESSION_CHANGE 4
#define B3_SECURITY_TRADING_EVENT_STATUS_CHANGE 101
#define B3_SECURITY_TRADING_EVENT_REJOINS_GROUP_STATUS 102

typedef uint8_t B3NewsSource;
#define B3_NEWS_SOURCE_OTHER 0
#define B3_NEWS_SOURCE_DCM 1
#define B3_NEWS_SOURCE_BBMNET 2
#define B3_NEWS_SOURCE_MARKET_SURVEILLANCE 3
#define B3_NEWS_SOURCE_INTERNET 4
#define B3_NEWS_SOURCE_DPR_VE 5
#define B3_NEWS_SOURCE_MKT_OPS_FX_AGENCY 19
#define B3_NEWS_SOURCE_MKT_OPS_DERIVATIVES_AGENCY 20
#define B3_NEWS_SOURCE_OVER_THE_COUNTER_AGENCY 11
#define ELECTRONIC_PURCHASE_EXCHANGE 13
#define CBLC_NEWS_AGENCY 14
#define BOVESPA_INDEX_AGENCY 15
#define BOVESPA_INSTITUTIONAL_AGENCY 16
#define MKT_OPS_EQUITIES_AGENCY 17
#define BOVESPA_COMPANIES_AGENCY 18

typedef uint8_t B3TradingSessionSubID;
#define PAUSE 2
#define CLOSE 4
#define OPEN 17
#define FORBIDDEN 18
#define UNKNOWN 20
#define RESERVED 21
#define CLOSING_CALL 101

typedef uint8_t B3LotType;
#define B3_LOT_TYPE_ODD 1
#define B3_LOT_TYPE_ROUND 2
#define B3_LOT_TYPE_BLOCK 3

typedef uint8_t B3Product;
#define COMMODITY 2
#define CORPORATE 3
#define CURRENCY 4
#define EQUITY 5
#define GOVERNMENT 6
#define INDEX 7
#define ECONOMIC_INDICATOR 15
#define MULTILEG 16

typedef uint8_t B3ExerciseStyle;
#define B3_EXERCISE_STYLE_EUROPEAN 0
#define B3_EXERCISE_STYLE_AMERICAN 1

typedef uint8_t B3PutOrCall;
#define B3_PUT 0
#define B3_CALL 1

typedef uint8_t B3PriceType;
#define B3_PRICE_TYPE_PERCENTAGE 1
#define B3_PRICE_TYPE_PU 2
#define B3_PRICE_TYPE_FIXED 3

typedef uint8_t B3GovernanceIndicator;
#define B3_GOVERNANCE_NO 0
#define B3_GOVERNANCE_N1 1
#define B3_GOVERNANCE_N2 2
#define B3_GOVERNANCE_NM 4
#define B3_GOVERNANCE_MA 5

typedef uint8_t B3MDUpdateAction;
#define B3_MDUPDATE_NEW 0
#define B3_MDUPDATE_CHANGE 1
#define B3_MDUPDATE_DELETE 2
#define B3_MDUPDATE_DELETE_THRU 3
#define B3_MDUPDATE_DELETE_FROM 4
/* There's a note on the docs saying that overlay is not used. */
#define B3_MDUPDATE_OVERLAY 5

/* I don't know, at least by now, why folks on the exchange use this
 * single-value enum but the procotol requires so I'm implementing. What's
 * probably even more interesting is that a lot of stuff in the structs are
 * actually "implied" because they're constant values. */
typedef uint8_t B3SecurityMatchType;
#define B3_SECURITY_MATCH_TYPE_ISSUING_BUY_BACK 8

typedef uint8_t B3MultiLegModel;
#define B3_MULTI_LEG_PREDEFINED 0
#define B3_MULTI_LEG_USER_DEFINED 1

typedef uint8_t B3SecurityType;
#define B3_SECURITY_TYPE_CASH 1
#define B3_SECURITY_TYPE_CORP 2
#define B3_SECURITY_TYPE_CS 3
#define B3_SECURITY_TYPE_DTERM 4
#define B3_SECURITY_TYPE_ETF 5
#define B3_SECURITY_TYPE_FOPT 6
#define B3_SECURITY_TYPE_FORWARD 7
#define B3_SECURITY_TYPE_FUT 8
#define B3_SECURITY_TYPE_INDEX 9
#define B3_SECURITY_TYPE_INDEXOPT 10
#define B3_SECURITY_TYPE_MLEG 11
#define B3_SECURITY_TYPE_OPT 12
#define B3_SECURITY_TYPE_OPTEXER 13
#define B3_SECURITY_TYPE_PS 14
#define B3_SECURITY_TYPE_SECLOAN 15
#define B3_SECURITY_TYPE_SOPT 16
#define B3_SECURITY_TYPE_SPOT 17

typedef uint8_t B3Side;
#define B3_BUY 1
#define B3_SELL 2

typedef uint8_t B3InstrAttribType;
#define B3_INSTR_ATTRIB_TYPE_TRADE_TYPE 24
#define B3_INSTR_ATTRIB_GTD_GTC 34

typedef uint8_t B3InstrAttribValue;
#define B3_INSTR_ATTRIB_VALUE_ELECTRONIC_MATCH_OR_GTD_GTC 1
#define B3_INSTR_ATTRIB_VALUE_ORDER_CROSS 2
#define B3_INSTR_ATTRIB_VALUE_BLOCK_TRADE 3
#define B3_INSTR_ATTRIB_VALUE_FLAG_RFQ_FOR_CROSS 14
#define B3_INSTR_ATTRIB_VALUE_NEGOTIATED_QUOTE 17

typedef uint8_t B3OpenCloseSettlFlag;
#define B3_OPENCLOSE_SETTL_DAILY 0
#define B3_OPENCLOSE_SETTL_SESSION 1
#define B3_OPENCLOSE_SETTL_EXPECTED_ENTRY 3
#define B3_OPENCLOSE_SETTL_ENTRY_FROM_PREV_DAY 4
#define B3_OPENCLOSE_SETTL_THEORETICAL_PRICE 5

typedef uint8_t B3PriceBandType;
#define B3_PRICE_BAND_HARD_LIMIT 1
#define B3_PRICE_BAND_AUCTION_LIMITS 2
#define B3_PRICE_BAND_REJECTION_BAND 3
#define B3_PRICE_BAND_STATIC_LIMITS 4

typedef uint8_t B3PriceLimitType;
#define B3_PRICE_LIMIT_UNIT 0
#define B3_PRICE_LIMIT_TICKS 1
#define B3_PRICE_LIMIT_PERCENTAGE 2

typedef uint8_t B3PriceBandMPType;
#define B3_PRICE_BAND_MP_LAST_TRADED 0
#define B3_PRICE_BAND_MP_COMPLEMENTARY_LAST 1
#define B3_PRICE_BAND_MP_THEORETICAL_PRICE 2

typedef uint8_t B3TrdSubType;
#define B3_TRD_SUBTYPE_MULTI_ASSET 101
#define B3_TRD_SUBTYPE_LEG 102
#define B3_TRD_SUBTYPE_MIDPOINT 103
#define B3_TRD_SUBTYPE_BLOCK_BOOK 104
#define B3_TRD_SUBTYPE_RF 105
#define B3_TRD_SUBTYPE_RLP 106
#define B3_TRD_SUBTYPE_TAC 107
#define B3_TRD_SUBTYPE_TAA 108

typedef uint8_t B3AggressorSide;
#define B3_NO_AGGRESSOR 0
#define B3_AGGRESSOR_SIDE_BUY 1
#define B3_AGGRESSOR_SIDE_SELL 2

/*
 * Primitives.
 */

typedef uint8_t B3CFICode[6];
typedef uint8_t B3Asset[6];
typedef uint8_t B3CountryCode[2];
typedef uint8_t B3Currency[3];
typedef uint8_t B3ISINNumber[12];
typedef uint8_t B3LanguageCode[2];
typedef uint8_t B3SecurityExchange[4];
typedef uint8_t B3SecurityGroup[3];
typedef uint8_t B3SecurityStrategyType[3];
typedef uint8_t B3Symbol[20];

typedef uint8_t B3ChannelNumber;
typedef uint8_t B3Day;
typedef uint8_t B3MarketSegmentID;
typedef uint8_t B3Month;
typedef uint8_t B3Week;

typedef uint16_t B3Utf8StringLength;
typedef uint16_t B3LocalMktDate;
typedef uint16_t B3MessageLength;
typedef uint16_t B3PacketLength;
typedef uint16_t B3SequenceVersion;
typedef uint16_t B3SettlType;
typedef uint16_t B3Year;

typedef int32_t B3LocalMktDate32;

typedef uint32_t B3Firm;
typedef uint32_t B3MDEntryPositionNo;
typedef uint32_t B3NumberOfTrades;
typedef uint32_t B3RptSeq;
typedef uint32_t B3SeqNum;
typedef uint32_t B3TradeID;

typedef int64_t B3Quantity;
typedef int64_t B3QuantityVolume;
typedef int64_t B3Fixed8;
typedef int64_t B3Price;
typedef int64_t B3RatioQty;
typedef int64_t B3Price8;
typedef int64_t B3PriceOffset8;

typedef uint64_t B3ClearingHouseID;
typedef uint64_t B3NewsID;
typedef uint64_t B3OrderID;
typedef uint64_t B3SecurityID;
typedef uint64_t B3Time;
typedef uint64_t B3UTCTimestampSeconds;
typedef uint64_t B3UTCTimestampNanos;
typedef uint64_t B3Percentage;
typedef uint64_t B3Percentage9;

typedef struct {
	uint8_t counter;
	uint16_t block_length;
}__attribute__((packed)) B3NumInGroup;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
}__attribute__((packed)) B3MaturityMonthYear;

/*
 * Messages.
 */

typedef struct {
	B3SeqNum next_sequence_number;
}__attribute__((packed)) B3MessageSequence;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3SecurityTradingStatus security_trading_status;
	B3SecurityTradingEvent security_trading_event;
	B3LocalMktDate trade_date;
	uint16_t __padding__;
	B3UTCTimestampNanos trade_session_open_time;
	B3UTCTimestampNanos transaction_time;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageSecurityStatus;

/* It's important to note that this does not contain the news itself, as it's
 * impossible to represent it as a struct in ISO C90. */
typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3NewsSource news_source;
	B3LanguageCode language_code;
	uint16_t part_count;
	uint16_t part_number;
	B3NewsID news_id;
	B3UTCTimestampNanos orig_time;
	uint32_t total_text_length;
}__attribute__((packed)) B3MessageNews;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	uint16_t __padding__;
	uint8_t __padding2__;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3MessageEmptyBook;

typedef struct {
	B3SecurityGroup security_group;
	uint32_t __padding__;
	uint8_t __padding2__;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3TradingSessionSubID trading_session_subid;
	B3SecurityTradingEvent security_trading_event;
	B3LocalMktDate trade_date;
	uint16_t __padding3__;
	B3UTCTimestampNanos trade_session_open_time;
	B3UTCTimestampNanos transaction_time;
}__attribute__((packed)) B3MessageSecurityGroupPhase;

typedef struct {
	B3MatchEventIndicator match_event_indicator;
	uint16_t __padding__;
	uint8_t __padding2__;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3MessageChannelReset;

typedef struct {
	B3SecurityID security_id;
	B3SecurityExchange security_exchange;
	B3SecurityIDSource security_id_source;
	B3SecurityGroup security_group;
	B3Symbol symbol;
	B3SecurityUpdateAction security_update_action;
	B3SecurityType security_type;
	uint16_t security_subtype;
	uint32_t number_related_symbols;
	B3Fixed8 minimum_price_increments;
	B3Price strike_price;
	B3Fixed8 contract_multiplier;
	B3Fixed8 price_divisor;
	B3UTCTimestampSeconds security_validity_timestamp;
	uint64_t no_shares_issued;
	B3ClearingHouseID clearing_house_id;
	B3Quantity min_order_qty;
	B3Quantity max_order_qty;
	B3Quantity min_lot_size;
	B3Quantity min_trade_vol;
	uint32_t corporate_action_event_id;
	B3LocalMktDate issue_date;
	B3LocalMktDate maturity_date;
	B3CountryCode country_of_issue;
	B3LocalMktDate start_date;
	B3LocalMktDate end_date;
	B3SettlType settl_type;
	B3LocalMktDate settl_date;
	B3LocalMktDate dated_date;
	B3ISINNumber isin_number;
	B3Asset asset;
	B3CFICode cfi_code;
	B3MaturityMonthYear maturity_month_year;
	B3MaturityMonthYear contract_settl_month;
	B3Currency currency;
	B3Currency strike_currency;
	B3Currency settl_currency;
	B3SecurityStrategyType security_strategy_type;
	B3LotType lot_type;
	uint8_t tick_size_denominator;
	B3Product product;
	B3ExerciseStyle exercise_style;
	B3PutOrCall put_or_call;
	B3PriceType price_type;
	B3MarketSegmentID market_segment_id;
	B3GovernanceIndicator governance_indicator;
	B3SecurityMatchType security_match_type;
	uint8_t last_fragment;
	B3MultiLegModel multi_leg_model;
	B3Quantity min_cross_qty;
	struct {
		B3SecurityID security_id;
		B3Symbol symbol;
	}__attribute((packed)) no_underlyings;
	struct {
		B3SecurityID security_id;
		B3RatioQty ratio_qty;
		B3SecurityType security_type;
		B3Side side;
		B3Symbol symbol;
	}__attribute((packed)) no_legs;
	struct {
		B3InstrAttribType instr_attrib_type;
		B3InstrAttribValue instr_attrib_value;
	}__attribute((packed)) no_instr_attrib;
}__attribute__((packed)) B3MessageSecurityDefinition;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3OpenCloseSettlFlag openclose_settl_flag;
	uint8_t __padding__;
	B3Price md_entry_px;
	B3PriceOffset8 net_chg_prev_day;
	B3LocalMktDate trade_date;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageOpeningPrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3LocalMktDate trade_date;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageTheoreticalOpeningPrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3OpenCloseSettlFlag open_close_settl;
	uint16_t __padding__;
	B3Price8 md_entry_px;
	B3LocalMktDate last_trade_date;
	B3LocalMktDate trade_date;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageClosingPrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3ImbalanceCondition imbalance_condition;
	B3Quantity md_entry_size;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageAuctionImbalance;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3PriceBandType price_band_type;
	B3PriceLimitType price_limit_type;
	B3PriceBandMPType price_band_mp_type;
	B3Price low_limit_price;
	B3Price high_limit_price;
	B3Fixed8 trading_reference_price;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessagePriceBand;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	uint16_t __padding__;
	uint8_t __padding2__;
	B3QuantityVolume avg_daily_traded_qty;
	B3QuantityVolume max_traded_volume;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageQuantityBand;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3LocalMktDate trade_date;
	B3Price md_entry_px;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageHighPrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3LocalMktDate trade_date;
	B3Price md_entry_px;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageLowPrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3TradeCondition trade_condition;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3TradeID trade_id;
	B3Firm md_entry_buyer;
	B3Firm md_entry_seller;
	B3LocalMktDate trade_date;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
	uint16_t seller_days;
	B3Percentage md_entry_interest_rate;
	B3TrdSubType trd_sub_type;
	uint16_t __padding__;
	uint8_t __padding2__;
}__attribute__((packed)) B3MessageLastTradePrice;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3MDEntryType md_entry_type;
	uint8_t __padding__;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3MDEntryPositionNo md_entry_position_no;
	B3Firm entering_firm;
	B3UTCTimestampNanos md_insert_timestamp;
	B3OrderID secondary_order_id;
	B3RptSeq rpt_seq;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3MessageOrderMBO;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	uint8_t __padding__;
	B3MDEntryType md_entry_type;
	uint8_t __padding2__;
	B3MDEntryPositionNo md_entry_position_no;
	B3Quantity md_entry_size;
	B3OrderID secondary_order_id;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageDeleteOrderMBO;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3MDUpdateAction md_update_action;
	B3MDEntryType md_entry_type;
	uint8_t __padding__;
	B3MDEntryPositionNo md_entry_position_no;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageMassDeleteOrdersMBO;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3TradeCondition trade_condition;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3TradeID trade_id;
	B3Firm md_entry_buyer;
	B3Firm md_entry_seller;
	B3LocalMktDate trade_date;
	B3TrdSubType trd_sub_type;
	uint8_t __padding__;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageTrade;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3TradeCondition trade_condition;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3TradeID trade_id;
	B3Firm md_entry_buyer;
	B3Firm md_entry_seller;
	B3LocalMktDate trade_date;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
	uint16_t seller_days;
	B3Percentage md_entry_interest_rate;
	B3TrdSubType trd_sub_type;
	uint16_t __padding__;
	uint8_t __padding2__;
}__attribute__((packed)) B3MessageForwardTrade;

typedef struct {
	B3SecurityID security_id;
	uint16_t __padding__;
	B3AggressorSide aggressor_side;
	uint8_t __padding2__;
	B3Price last_px;
	B3Quantity fill_qty;
	B3Quantity tradded_hidden_qty;
	B3Quantity cxl_qty;
	B3UTCTimestampNanos aggressor_time;
	B3RptSeq rpt_seq;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3MessageExecutionSummary;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3LocalMktDate trade_date;
	B3QuantityVolume trade_volume;
	B3Price vwap_px;
	B3PriceOffset8 net_chg_prev_day;
	B3NumberOfTrades number_of_trades;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageExecutionStatistics;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	uint16_t __padding__;
	B3Price md_entry_px;
	B3Quantity md_entry_size;
	B3TradeID trade_id;
	B3LocalMktDate trade_date;
	uint16_t __padding2__;
	B3UTCTimestampNanos md_entry_timestamp;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3MessageTradeBust;

typedef struct {
	B3SecurityID security_id;
	B3SeqNum last_msg_seq_num_processed;
	uint32_t tot_num_reports;
	uint32_t tot_num_bids;
	uint32_t tot_num_offers;
	uint16_t tot_num_stats;
	uint16_t __padding__;
	B3RptSeq last_rpt_seq;
}__attribute__((packed)) B3MessageSnapshotFullRefreshHeader;

typedef struct {
	B3SecurityID security_id;
	struct {
		B3Price md_entry_px;
		B3Quantity md_entry_size;
		B3MDEntryPositionNo md_entry_position_no;
		B3Firm entering_firm;
		B3UTCTimestampNanos md_insert_timestamp;
		B3OrderID secondary_order_id;
		B3MDEntryType md_entry_type;
	}__attribute__((packed)) no_md_entries;
}__attribute__((packed)) B3SnapshotFullRefreshOrdersMBO;

/*
 * Private messages.
 */

#define B3TP_NEGOTIATE 1
#define B3TP_NEGOTIATE_RESPONSE 2
#define B3TP_NEGOTIATE_REJECT 3
#define B3TP_ESTABILISH 4
#define B3TP_ESTABILISH_ACK 5
#define B3TP_ESTABILISH_REJECT 6
#define B3TP_TERMINATE 7
#define B3TP_NOT_APPLIED 8
#define B3TP_SEQUENCE 9
#define B3TP_RETRANSMIT_REQUEST 12
#define B3TP_RETRANSMISSION 13
#define B3TP_RETRANSMIT_REJECT 14
#define B3TP_SIMPLE_NEW_ORDER 100
#define B3TP_SIMPLE_MODIFY_ORDER 101
#define B3TP_NEW_ORDER_SINGLE 102
#define B3TP_ORDER_CANCEL_REPLACE_REQUEST 104
#define B3TP_ORDER_CANCEL_REQUEST 105
#define B3TP_NEW_ORDER_CROSS 106
#define B3TP_EXECUTION_REPORT_NEW 200
#define B3TP_EXECUTION_REPORT_MODIFY 201
#define B3TP_EXECUTION_REPORT_CANCEL 2O2
#define B3TP_EXECUTION_REPORT_TRADE 203
#define B3TP_EXECUTION_REPORT_REJECT 2O4
#define B3TP_EXECUTION_REPORT_FORWARD 205
#define B3TP_BUSINESS_MESSAGE_REJECT 206
#define B3TP_SECURITY_DEFINITION_REQUEST 300
#define B3TP_SECURITY_DEFINITION_RESPONSE 301
#define B3TP_QUOTE_REQUEST 401
#define B3TP_QUOTE_STATUS_REPORT 402
#define B3TP_QUOTE 403
#define B3TP_QUOTE_CANCEL 404
#define B3TP_QUOTE_REQUEST_REJECT 405
#define B3TP_POSITION_MAINTENANCE_CANCEL_REQUEST 501
#define B3TP_POSITION_MAINTENANCE_REQUEST 502
#define B3TP_POSITION_MAINTENANCE_REPORT 503
#define B3TP_ALLOCATION_INSTRUCTION 601
#define B3TP_ALLOCATION_REPORT 602
#define B3TP_ORDER_MASS_ACTION_REQUEST 701

typedef uint8_t B3NegotiationRejectCode;
#define B3_NEGOTIATION_REJECT_UNSPECIFIED 0
#define B3_NEGOTIATION_REJECT_CREDENTIALS 1
#define B3_NEGOTIATION_REJECT_FLOWTYPE_NOT_SUPPORTED 2
#define B3_NEGOTIATION_REJECT_ALREADY_NEGOTIATED 3
#define B3_NEGOTIATION_REJECT_SESSION_BLOCKED 4
#define B3_NEGOTIATION_REJECT_INVALID_SESSIONID 5
#define B3_NEGOTIATION_REJECT_INVALID_SESSIONVERID 6
#define B3_NEGOTIATION_REJECT_INVALID_TIMESTAMP 7
#define B3_NEGOTIATION_REJECT_INVALID_FIRM 8
#define B3_NEGOTIATION_REJECT_NEGOTIATE_NOT_ALLOWED 20
#define B3_NEGOTIATION_REJECT_DUPLICATE_SESSION_CONNECTION 21
#define B3_NEGOTIATION_REJECT_AUTHENTICATION_IN_PROGRESS 22
#define B3_NEGOTIATION_REJECT_PROTOCOL_VERSION_NOT_SUPPORTED 23

typedef uint8_t B3CancelOnDisconnectType;
#define B3_CANCEL_ON_DISCONNECT_DO_NOT_OR_TERMINATE 0
#define B3_CANCEL_ON_DISCONNECT_ONLY 1
#define B3_CANCEL_ON_TERMINATE_ONLY 2
#define B3_CANCEL_ON_DISCONNECT_OR_TERMINATE 3

typedef uint8_t B3EstabilishRejectCode;
#define B3_ESTABILISH_REJECT_UNSPECIFIED 0
#define B3_ESTABILISH_REJECT_CREDENTIALS 1
#define B3_ESTABILISH_REJECT_UNNEGOTIATED 2
#define B3_ESTABILISH_REJECT_ALREADY_ESTABLISHED 3
#define B3_ESTABILISH_REJECT_SESSION_BLOCKED 4
#define B3_ESTABILISH_REJECT_INVALID_SESSIONID 5
#define B3_ESTABILISH_REJECT_INVALID_SESSIONVERID 6
#define B3_ESTABILISH_REJECT_INVALID_TIMESTAMP 7
#define B3_ESTABILISH_REJECT_INVALID_KEEPALIVE_INTERVAL 8
#define B3_ESTABILISH_REJECT_INVALID_NEXTSEQNO 9
#define B3_ESTABILISH_REJECT_ATTEMPTS_EXCEEDED 10
#define B3_ESTABILISH_REJECT_NOT_ALLOWED 20
#define B3_ESTABILISH_REJECT_DUPLICATE_SESSION_CONNECTION 21
#define B3_ESTABILISH_REJECT_AUTHENTICATION_IN_PROGRESS 22
#define B3_ESTABILISH_REJECT_PROTOCOL_VERSION_NOT_SUPPORTED 23

typedef uint8_t B3RetransmitRejectCode;
#define B3_RETRANSMIT_REJECT_OUT_OF_RANGE 0
#define B3_RETRANSMIT_REJECT_INVALID_SESSION 1
#define B3_RETRANSMIT_REJECT_REQUEST_LIMIT_EXCEEDED 2
#define B3_RETRANSMIT_REJECT_RETRANSMIT_IN_PROGRESS 3
#define B3_RETRANSMIT_REJECT_INVALID_TIMESTAMP 4
#define B3_RETRANSMIT_REJECT_INVALID_FROMSEQNO 5
#define B3_RETRANSMIT_REJECT_INVALID_COUNT 9
#define B3_RETRANSMIT_REJECT_THROTTLE_REJECT 10
#define B3_RETRANSMIT_REJECT_SYSTEM_BUSY 11

typedef uint8_t B3TerminationCode;
#define B3_TERMINATION_UNSPECIFIED 0
#define B3_TERMINATION_FINISHED 1
#define B3_TERMINATION_UNNEGOTIATED 2
#define B3_TERMINATION_NOT_ESTABLISHED 3
#define B3_TERMINATION_SESSION_BLOCKED 4
#define B3_TERMINATION_NEGOTIATION_IN_PROGRESS 5
#define B3_TERMINATION_ESTABLISH_IN_PROGRESS 6
#define B3_TERMINATION_KEEPALIVE_INTERVAL_LAPSED 10
#define B3_TERMINATION_INVALID_SESSIONID 11
#define B3_TERMINATION_INVALID_SESSIONVERID 12
#define B3_TERMINATION_INVALID_TIMESTAMP 13
#define B3_TERMINATION_INVALID_NEXTSEQNO 14
#define B3_TERMINATION_UNRECOGNIZED_MESSAGE 15
#define B3_TERMINATION_INVALID_SOFH 16
#define B3_TERMINATION_DECODING_ERROR 17
#define B3_TERMINATION_NOT_ALLOWED 20
#define B3_TERMINATION_IN_PROGRESS 21
#define B3_TERMINATION_PROTOCOL_VERSION_NOT_SUPPORTED 23
#define B3_TERMINATION_BACKUP_TAKEOVER_IN_PROGRESS 30

typedef uint8_t B3SelfTradePreventionInstruction;
#define B3_SELF_TRADE_PREVENTION_NONE 0
#define B3_SELF_TRADE_PREVENTION_AGGRESSOR 1
#define B3_SELF_TRADE_PREVENTION_RESTING 2
#define B3_SELF_TRADE_PREVENTION_BOTH 3

typedef uint8_t B3RoutingInstruction;
#define B3_ROUTING_RETAIL_LIQUIDITY_TAKER 1
#define B3_ROUTING_WAIVED_PRIORITY 2
#define B3_ROUTING_BROKER_ONLY 3
#define B3_ROUTING_BROKER_ONLY_REMOVAL 4

typedef uint8_t B3ExecRRVFSC;
#define B3_EXEC_RRVFSC_CANCEL_DUE_TO_OP_ERROR 203

typedef uint8_t B3ExecRestatementReason;
#define B3_EXEC_RESTATEMENT__MARKET_OPTION 8
#define B3_EXEC_RESTATEMENT_CANCEL_ON_HARD_DISCONNECTION 100
#define B3_EXEC_RESTATEMENT_CANCEL_ON_TERMINATE 101
#define B3_EXEC_RESTATEMENT_CANCEL_ON_DISCONNECT_AND_TERMINATE 102
#define B3_EXEC_RESTATEMENT_SELF_TRADING_PREVENTION 103
#define B3_EXEC_RESTATEMENT_CANCEL_FROM_FIRMSOFT 105
#define B3_EXEC_RESTATEMENT_CANCEL_RESTING_ORDER_ON_SELF_TRADE 107
#define B3_EXEC_RESTATEMENT_MARKET_MAKER_PROTECTION 200
#define B3_EXEC_RESTATEMENT_RISK_MANAGEMENT_CANCELLATION 201
#define B3_EXEC_RESTATEMENT_ORDER_MASS_ACTION_FROM_CLIENT_REQUEST 202
#define B3_EXEC_RESTATEMENT_CANCEL_ORDER_DUE_TO_OP_ERROR 203
#define B3_EXEC_RESTATEMENT_ORDER_CANCELLED_DUE_TO_OP_ERROR 204

/* I don't know why this exists. */
typedef uint8_t B3PossResend;
#define B3_POSS_RESEND_FALSE 0
#define B3_POSS_RESEND_TRUE 1

/* Same. */
typedef uint8_t B3ExecuteUnderlyingTrade;
#define B3_EXECUTE_UNDERLYING_TRADE_NO 0
#define B3_EXECUTE_UNDERLYING_TRADE_OPPOSING 1

typedef uint8_t B3AccountType;
#define B3_ACCOUNT_TYPE_REMOVE_INFO 38
#define B3_ACCOUNT_TYPE_REGULAR 39

typedef uint8_t B3CxlRejResponseTo;
#define B3_CXL_REJ_RESPONSE_TO_NEW 0
#define B3_CXL_REJ_RESPONSE_TO_CANCEL 1
#define B3_CXL_REJ_RESPONSE_TO_REPLACE 2

typedef uint8_t B3ExecRRVFMC;
#define B3_EXEC_RRVFMC_ORDER_MASS_ACTION 202
#define B3_EXEC_RRVFMC_ORDER_MASS_CANCEL 207

typedef uint8_t B3MessageType;
#define B3_MESSAGE_TYPE_NEGOTIATE 0
#define B3_MESSAGE_TYPE_NEGOTIATE_RESPONSE 1
#define B3_MESSAGE_TYPE_NEGOTIATE_REJECT 2
#define B3_MESSAGE_TYPE_ESTABLISH 3
#define B3_MESSAGE_TYPE_ESTABLISH_ACK 4
#define B3_MESSAGE_TYPE_ESTABLISH_REJECT 5
#define B3_MESSAGE_TYPE_TERMINATE 6
#define B3_MESSAGE_TYPE_NOT_APPLIED 9
#define B3_MESSAGE_TYPE_RETRANSMIT_REQUEST 10
#define B3_MESSAGE_TYPE_RETRANSMISSION 11
#define B3_MESSAGE_TYPE_RETRANSMIT_REJECT 12
#define B3_MESSAGE_TYPE_SEQUENCE 13
#define B3_MESSAGE_TYPE_BUSINESS_MESSAGE_REJECT 14
#define B3_MESSAGE_TYPE_SIMPLE_NEW_ORDER 15
#define B3_MESSAGE_TYPE_SIMPLE_MODIFY_ORDER 16
#define B3_MESSAGE_TYPE_NEW_ORDER_SINGLE 17
#define B3_MESSAGE_TYPE_ORDER_CANCEL_REPLACE_REQUEST 18
#define B3_MESSAGE_TYPE_ORDER_CANCEL_REQUEST 19
#define B3_MESSAGE_TYPE_NEW_ORDER_CROSS 20
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_NEW 21
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_MODIFY 22
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_CANCEL 23
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_TRADE 24
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_REJECT 25
#define B3_MESSAGE_TYPE_EXECUTION_REPORT_FORWARD 26
#define B3_MESSAGE_TYPE_SECURITY_DEFINITION_REQUEST 27
#define B3_MESSAGE_TYPE_SECURITY_DEFINITION_RESPONSE 28
#define B3_MESSAGE_TYPE_ORDER_MASS_ACTION_REQUEST 29
#define B3_MESSAGE_TYPE_ORDER_MASS_ACTION_REPORT 30
#define B3_MESSAGE_TYPE_QUOTE_REQUEST 31
#define B3_MESSAGE_TYPE_QUOTE_STATUS_REPORT 32
#define B3_MESSAGE_TYPE_QUOTE 33
#define B3_MESSAGE_TYPE_QUOTE_CANCEL 34
#define B3_MESSAGE_TYPE_QUOTE_REQUEST_REJECT 35
#define B3_MESSAGE_TYPE_POSITION_MAINTENANCE_CANCEL_REQUEST 36
#define B3_MESSAGE_TYPE_POSITION_MAINTENANCE_REQUEST 37
#define B3_MESSAGE_TYPE_POSITION_MAINTENANCE_REPORT 38
#define B3_MESSAGE_TYPE_ALLOCATION_INSTRUCTION 39
#define B3_MESSAGE_TYPE_ALLOCATION_REPORT 40

typedef uint8_t B3MassActionType;
#define B3_MASS_ACTION_TYPE_RELEASE_FROM_SUSPENSION 2
#define B3_MASS_ACTION_TYPE_CANCEL 3
#define B3_MASS_ACTION_TYPE_CANCEL_AND_SUSPEND 4
#define B3_MASS_ACTION_TYPE_SESSION_GROUP_QUERY 5

typedef uint8_t B3MassActionScope;
#define B3_MASS_ACTION_SCOPE_ALL_FOR_SESSION 6

typedef uint8_t B3MassActionRejectReason;
#define B3_MASS_ACTION_REJ_REASON_NOT_SUPPORTED 0
#define B3_MASS_ACTION_REJ_REASON_INVALID_MS 8
#define B3_MASS_ACTION_REJ_REASON_OTHER 99

typedef uint8_t B3SecurityResponseType;
#define B3_SECURITY_RESPONSE_ACCEPT 1
#define B3_SECURITY_RESPONSE_ACCEPT_WITH_REVISIONS 2
#define B3_SECURITY_RESPONSE_REJECT 5

typedef uint8_t B3QuoteStatus;
#define B3_QUOTE_EXPIRED 7
#define B3_QUOTE_ACCEPTED 0
#define B3_QUOTE_REJECTED 5
#define B3_QUOTE_QUOTE_NOT_FOUND 9
#define B3_QUOTE_PENDING 10
#define B3_QUOTE_PASS 11
#define B3_QUOTE_CANCELED 17

typedef uint8_t B3PosTransType;
#define B3_POS_TRANS_EXERCISE 1
#define B3_POS_TRANS_AUTOMATIC_EXERCISE 105
#define B3_POS_TRANS_EXERCISE_NOT_AUTOMATIC 106

typedef uint8_t B3MassActionResponse;
#define B3_MASS_ACTION_RESPONSE_REJECTED ((uint8_t) '0')
#define B3_MASS_ACTION_RESPONSE_ACCEPTED ((uint8_t) '1')

typedef uint8_t B3SimpleOrdType;
#define B3_SIMPLE_ORD_TYPE_MARKET ((uint8_t) '1')
#define B3_SIMPLE_ORD_TYPE_LIMIT ((uint8_t) '2')

typedef uint8_t B3SimpleTimeInForce;
#define B3_SIMPLE_TIME_IN_FORCE_DAY ((uint8_t) '0')
#define B3_SIMPLE_TIME_IN_FORCE_IMM_OR_CANCEL ((uint8_t) '3')
#define B3_SIMPLE_TIME_IN_FORCE_FILL_OR_KILL ((uint8_t) '4')

typedef uint8_t B3TimeInForce;
#define B3_TIME_IN_FORCE_DAY ((uint8_t) '0')
#define B3_TIME_IN_FORCE_GOOD_TILL_CANCEL ((uint8_t) '1')
#define B3_TIME_IN_FORCE_IMM_OR_CANCEL ((uint8_t) '3')
#define B3_TIME_IN_FORCE_FILL_OR_KILL ((uint8_t) '4')

typedef uint8_t B3OrdStatus;
#define B3_ORD_STATUS_NEW ((uint8_t) '0')
#define B3_ORD_STATUS_PARTIALLY_FILLED ((uint8_t) '1')
#define B3_ORD_STATUS_FILLED ((uint8_t) '2')
#define B3_ORD_STATUS_CANCELED ((uint8_t) '4')
#define B3_ORD_STATUS_REPLACED ((uint8_t) '5')
#define B3_ORD_STATUS_REJECTED ((uint8_t) '8')
#define B3_ORD_STATUS_EXPIRED ((uint8_t) 'C')
#define B3_ORD_STATUS_RESTATED ((uint8_t) 'R')
#define B3_ORD_STATUS_PREVIOUS_FINAL_STATE ((uint8_t) 'Z')

typedef uint8_t B3OrdType;
#define B3_ORD_MARKET ((uint8_t) '1')
#define B3_ORD_LIMIT ((uint8_t) '2')
#define B3_ORD_STOP_LOSS ((uint8_t) '3')
#define B3_ORD_STOP_LIMIT ((uint8_t) '4')
#define B3_ORD_MARKET_WITH_LEFTOVER_AS_LIMIT ((uint8_t) 'K')
#define B3_ORD_RLP ((uint8_t) 'W')
#define B3_ORD_PEGGED_MIDPOINT ((uint8_t) 'P')

typedef uint8_t B3MultiLegReportingType;
#define B3_MULTILEG_SINGLE ((uint8_t) '1')
#define B3_MULTILEG_INDIVIDUALLEG_OF_MULTILEG ((uint8_t) '2')
#define B3_MULTILEG ((uint8_t) '3')

typedef uint8_t B3ExecType;
#define B3_EXEC_TYPE_TRADE ((uint8_t) 'F')
#define B3_EXEC_TYPE_TRADE_CANCEL ((uint8_t) 'H')

typedef uint8_t B3OrderCategory;
#define B3_ORDER_CATEGORY_OPTIONS_EXERCISE ((uint8_t) 'B')
#define B3_ORDER_CATEGORY_ASSIGNMENT_FROM_AN_OPTIONS_EXERCISE ((uint8_t) 'C')
#define B3_ORDER_CATEGORY_AUTOMATIC_OPTIONS_EXERCISE ((uint8_t) 'D')
#define B3_ORDER_CATEGORY_MIDPOINT_ORDER ((uint8_t) 'E')
#define B3_ORDER_CATEGORY_BLOCK_BOOK_TRADE ((uint8_t) 'F')
#define B3_ORDER_CATEGORY_TRADE_AT_CLOSE ((uint8_t) 'G')
#define B3_ORDER_CATEGORY_TRADE_AT_AVERAGE ((uint8_t) 'H')

typedef uint8_t B3PosMaintAction;
#define B3_POS_MAINT_ACTION_NEW ((uint8_t) '1')
#define B3_POS_MAINT_ACTION_CANCEL ((uint8_t) '3')

typedef uint8_t B3PosMaintStatus;
#define B3_POS_MAINT_STATUS_ACCEPTED ((uint8_t) '0')
#define B3_POS_MAINT_STATUS_REJECTED ((uint8_t) '2')
#define B3_POS_MAINT_STATUS_COMPLETED ((uint8_t) '3')
#define B3_POS_MAINT_STATUS_NOT_EXECUTED ((uint8_t) '9')

typedef uint8_t B3PosType;
#define B3_POS_TYPE_TRANSACTION_QUANTITY ((uint8_t) 'T')
#define B3_POS_TYPE_START_OF_DAY_QTY ((uint8_t) 'S')
#define B3_POS_TYPE_OPTION_EXERCISE_QTY ((uint8_t) 'E')
#define B3_POS_TYPE_BLOCKED_QTY ((uint8_t) 'B')
#define B3_POS_TYPE_UNCOVERED_QTY ((uint8_t) 'U')
#define B3_POS_TYPE_COVERED_QTY ((uint8_t) 'C')

typedef uint8_t B3AllocTransType;
#define B3_ALLOC_TRANS_TYPE_NEW ((uint8_t) '0')
#define B3_ALLOC_TRANS_TYPE_CANCEL ((uint8_t) '2')

typedef uint8_t B3AllocType;
#define B3_ALLOC_TYPE_REQUEST_TO_INTERMEDIARY ((uint8_t) '8')

typedef uint8_t B3AllocReportType;
#define B3_ALLOC_REPORT_TYPE_REQUEST_TO_INTERMEDIARY ((uint8_t) '8')

typedef uint8_t B3AllocNoOrdersType;
#define B3_ALLOC_NO_ORDERS_TYPE_NOT_SPECIFIED ((uint8_t) '0')

typedef uint8_t B3AllocStatus;
#define B3_ALLOC_STATUS_ACCEPTED ((uint8_t) '0')
#define B3_ALLOC_STATUS_REJECTED_BY_INTERMEDIARY ((uint8_t) '5')

/* This enum is the reason why we need to enforce enum sizes. */
typedef uint16_t B3CrossedIndicator;
#define B3_CROSSED_STRUCTURED_TRANSACTION 1001
#define B3_CROSSED_OPERATIONAL_ERROR 1002
#define B3_CROSSED_TWAP_VWAP 1003

typedef uint8_t B3OrdTagID;
typedef uint8_t B3TotNoRelatedSym;

typedef uint8_t B3SenderLocation[10];
typedef uint8_t B3Trader[5];

typedef uint16_t B3DaysToSettlement;

typedef uint32_t B3SessionID;
typedef uint32_t B3MessageCounter;
typedef uint32_t B3Account;
typedef uint32_t B3RejReason;

typedef uint64_t B3SessionVerID;
typedef uint64_t B3DeltaInMillis;
typedef uint64_t B3ClOrdID;
typedef uint64_t B3CrossID;
typedef uint64_t B3ExecID;
typedef uint64_t B3MassActionReportID;
typedef uint64_t B3SendingTime;
typedef uint64_t B3BusinessRejectRefID;
typedef uint64_t B3SecurityReqRespID;
typedef uint64_t B3QuoteID;
typedef uint64_t B3PosReqID;
typedef uint64_t B3PosMaintRptID;
typedef uint64_t B3AllocID;
typedef uint64_t B3AllocReportID;

typedef int64_t B3PriceOffset;

typedef struct {
	B3SessionID session_id;
	B3SeqNum msg_seq_num;
	B3SendingTime sending_time;
	B3MarketSegmentID market_segment_id;
	uint8_t __padding__;
}__attribute__((packed)) B3InboundBusinessHeader;

typedef struct {
	uint16_t prefix;
	/* This is documented as "padding" but it's reserved for future use and
	 * shall be filled with 0. */
	uint16_t reserved;
	uint32_t document;
}__attribute__((packed)) B3InvestorID;

typedef struct {
	uint32_t custodian;
	uint32_t custody_account;
	uint32_t custody_allocation_type;
}__attribute__((packed)) B3CustodianInfo;

typedef struct {
	uint16_t block_length;
	uint8_t num_in_group;
}__attribute__((packed)) B3GroupSizeEncoding;

typedef struct {
	B3SessionID session_id;
	B3SeqNum msg_seq_num;
	B3SendingTime sending_time;
	B3PossResend poss_resend;
	uint8_t __padding__;
}__attribute__((packed)) B3OutboundBusinessHeader;

typedef struct {
	B3SessionID session_id;
	B3SeqNum msg_seq_num;
	B3SendingTime sending_time;
	B3PossResend poss_resend;
	B3MarketSegmentID market_segment_id;
	uint16_t __padding__;
}__attribute__((packed)) B3BidirectionalBusinessHeader;

/* Does not contain credentials, client IP, client app name and client app
 * version. */
typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos timestamp;
	B3Firm entering_firm;
	B3Firm on_behalf_firm;
}__attribute__((packed)) B3MessagePNegotiate;

typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos request_timestamp;
	B3Firm entering_firm;
}__attribute__((packed)) B3MessagePNegotiateResponse;

typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos request_timestamp;
	B3Firm entering_firm;
	B3NegotiationRejectCode reject_code;
	uint16_t __padding__;
	uint8_t __padding2__;
	B3SessionVerID current_session_ver_id;
}__attribute__((packed)) B3MessagePNegotiateReject;

/* Does not contain credentials. */
typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos timestamp;
	B3DeltaInMillis keep_alive_interval;
	B3SeqNum next_seq_no;
	B3CancelOnDisconnectType cod_type;
	uint8_t __padding__;
	B3DeltaInMillis cod_timeout_window;
}__attribute__((packed)) B3MessagePEstabilish;

typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos request_timestamp;
	B3DeltaInMillis keep_alive_interval;
	B3SeqNum next_seq_no;
	B3SeqNum last_incoming_seq_no;
}__attribute__((packed)) B3MessagePEstabilishAck;

typedef struct {
	B3SessionID session_id;
	B3SessionVerID session_ver_id;
	B3UTCTimestampNanos request_timestamp;
	B3EstabilishRejectCode reject_code;
	uint8_t __padding__;
	B3SeqNum last_incoming_seq_no;
}__attribute__((packed)) B3MessagePEstabilishReject;

typedef struct {
	B3SeqNum next_seq_no;
}__attribute__((packed)) B3MessagePSequence;

typedef struct {
	B3SeqNum from_seq_no;
	B3MessageCounter count;
}__attribute__((packed)) B3MessagePNotApplied;

typedef struct {
	B3SessionID session_id;
	B3UTCTimestampNanos timestamp;
	B3SeqNum from_seq_no;
	B3MessageCounter count;
}__attribute__((packed)) B3MessagePRetransmitRequest;

typedef struct {
	B3SessionID session_id;
	B3UTCTimestampNanos request_timestamp;
	B3SeqNum next_seq_no;
	B3MessageCounter count;
}__attribute__((packed)) B3MessagePRetransmission;

typedef struct {
	B3SessionID session_id;
	B3UTCTimestampNanos request_timestamp;
	B3RetransmitRejectCode code;
}__attribute__((packed)) B3MessagePRetransmitReject;

typedef struct {
	B3SessionID session_id;
	B3UTCTimestampNanos request_timestamp;
	B3TerminationCode code;
}__attribute__((packed)) B3MessagePTerminate;

/* Does not contain the memo. */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3OrdTagID ord_tag_id;
	uint8_t has_mm_protection_reset;
	B3ClOrdID cl_ord_id;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3SelfTradePreventionInstruction self_trade_pi;
	B3SecurityID security_id;
	B3Side side;
	B3SimpleOrdType ord_type;
	B3SimpleTimeInForce time_in_force;
	B3RoutingInstruction routing_instruction;
	B3Quantity quantity;
	B3Price price;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePSimpleNewOrder;

/* Same. */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3OrdTagID ord_tag_id;
	uint8_t has_mm_protection_reset;
	B3ClOrdID cl_ord_id;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3SelfTradePreventionInstruction self_trade_pi;
	B3SecurityID security_id;
	B3Side side;
	B3SimpleOrdType ord_type;
	B3SimpleTimeInForce time_in_force;
	B3RoutingInstruction routing_instruction;
	B3Quantity quantity;
	B3Price price;
	B3OrderID order_id;
	B3ClOrdID orig_cl_order_id;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePSimpleModifyOrder;

/* Does not contain desk id nor memo. */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3OrdTagID ord_tag_id;
	uint8_t has_mm_protection_reset;
	B3ClOrdID cl_ord_id;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3SelfTradePreventionInstruction self_trade_pi;
	B3SecurityID security_id;
	B3Side side;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3RoutingInstruction routing_instruction;
	B3Quantity quantity;
	B3Price price;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
	B3Trader executing_trader;
	B3LocalMktDate expire_date;
	B3CustodianInfo custodian_info;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePNewOrderSingle;

/* Same. */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3OrdTagID ord_tag_id;
	uint8_t has_mm_protection_reset;
	B3ClOrdID cl_ord_id;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3SelfTradePreventionInstruction self_trade_pi;
	B3SecurityID security_id;
	B3Side side;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3RoutingInstruction routing_instruction;
	B3Quantity quantity;
	B3Price price;
	B3OrderID order_id;
	B3ClOrdID orig_cl_order_id;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
	B3Trader executing_trader;
	B3AccountType account_type;
	B3LocalMktDate expire_date;
	B3CustodianInfo custodian_info;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePOrderCancelReplaceRequest;

/* Same. */
typedef struct {
	B3InboundBusinessHeader business_header;
	uint16_t __padding__;
	B3ClOrdID cl_ord_id;
	B3SecurityID security_id;
	B3OrderID order_id;
	B3ClOrdID orig_cl_ord_id;
	B3Side side;
	B3ExecRRVFSC exec_rrvfsc;
	uint16_t __padding2__;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Trader executing_trader;
}__attribute__((packed)) B3MessagePOrderCancelRequest;

/* Same. */
typedef struct {
	B3InboundBusinessHeader business_header;
	uint16_t __padding__;
	B3CrossID cross_id;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Trader executing_trader;
	B3SecurityID security_id;
	B3Quantity order_quantity;
	B3Price price;
	B3CrossedIndicator crossed_indicator;
	B3GroupSizeEncoding no_sides;
	B3Side side;
	uint8_t __padding2__;
	B3Account account;
	B3Firm entering_firm;
	B3ClOrdID cl_ord_id;
}__attribute__((packed)) B3MessagePNewOrderCross;

/* Same. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3OrderID order_id;
	B3Account account;
	B3ExecID exec_id;
	B3UTCTimestampNanos transact_time;
	B3UTCTimestampNanos market_segment_received_time;
	B3Price protection_price;
	B3LocalMktDate trade_date;
	uint8_t is_working;
	B3MultiLegReportingType multileg_reporting_type;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3LocalMktDate expire_date;
	B3Quantity order_quantity;
	B3Price price;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
	B3CrossID cross_id;
}__attribute__((packed)) B3MessagePExecutionReportNew;

/* Same. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3Quantity leaves_quantity;
	B3Account account;
	B3ExecID exec_id;
	B3UTCTimestampNanos transact_time;
	B3Quantity cum_quantity;
	B3UTCTimestampNanos market_segment_received_time;
	B3OrderID order_id;
	B3ClOrdID orig_cl_ord_id;
	B3Price protection_price;
	B3LocalMktDate trade_date;
	uint8_t is_working;
	B3MultiLegReportingType multileg_reporting_type;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3LocalMktDate expire_date;
	B3Quantity order_quantity;
	B3Price price;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
}__attribute__((packed)) B3MessagePExecutionReportModify;

/* Same. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3Quantity cum_quantity;
	B3Account account;
	B3ExecID exec_id;
	B3UTCTimestampNanos transact_time;
	B3UTCTimestampNanos market_segment_received_time;
	B3OrderID order_id;
	B3ClOrdID orig_cl_ord_id;
	B3LocalMktDate trade_date;
	uint8_t is_working;
	B3ExecRestatementReason exec_restatement_reason;
	uint32_t __padding__;
	B3MassActionReportID mass_action_report_id;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3LocalMktDate expire_date;
	B3Quantity order_quantity;
	B3Price price;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
}__attribute__((packed)) B3MessagePExecutionReportCancel;

/* Does not contain desk id, memo nor text. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3CxlRejResponseTo cxl_rej_response_to;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3RejReason ord_rej_reason;
	B3UTCTimestampNanos transact_time;
	B3ExecID exec_id;
	B3OrderID order_id;
	B3ClOrdID orig_ci_ord_id;
	B3Account account;
	B3OrdType ord_type;
	B3TimeInForce time_in_force;
	B3LocalMktDate expire_date;
	B3Quantity order_quantity;
	B3Price price;
	B3Price stop_px;
	B3Quantity min_quantity;
	B3Quantity max_floor;
	B3CrossID cross_id;
	B3CrossedIndicator crossed_indicator;
}__attribute__((packed)) B3MessagePExecutionReportReject;

/* Does not contain desk id nor memo. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3Account account;
	B3Quantity last_quantity;
	B3Price last_px;
	B3ExecID exec_id;
	B3UTCTimestampNanos transact_time;
	B3Quantity leaves_quantity;
	B3Quantity cum_quantity;
	uint8_t is_aggressor;
	B3ExecType exec_type;
	B3OrderCategory order_category;
	B3MultiLegReportingType multileg_reporting_type;
	B3TradeID trade_id;
	B3Firm contra_broker;
	B3OrderID order_id;
	B3LocalMktDate trade_date;
	B3TotNoRelatedSym tot_no_related_sym;
	uint8_t __padding__;
	B3ExecID secondary_exec_id;
	B3ExecID exec_ref_id;
	B3CrossID cross_id;
	B3CrossedIndicator crossed_indicator;
	B3Quantity order_quantity;
}__attribute__((packed)) B3MessagePExecutionReportTrade;

/* Same. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3Side side;
	B3OrdStatus ord_status;
	B3ClOrdID cl_ord_id;
	B3OrderID secondary_order_id;
	B3SecurityID security_id;
	B3Account account;
	B3Quantity last_quantity;
	B3Price last_px;
	B3ExecID exec_id;
	B3UTCTimestampNanos transact_time;
	B3Quantity leaves_quantity;
	B3Quantity cum_quantity;
	B3TradeID trade_id;
	B3Firm contra_broker;
	B3OrderID order_id;
	uint8_t is_aggressor;
	B3SettlType settl_type;
	B3LocalMktDate trade_date;
	B3DaysToSettlement days_to_settlement;
	uint16_t __padding__;
	B3ExecID secondary_exec_id;
	B3ExecID exec_ref_id;
	B3Percentage fixed_rate;
	B3Quantity order_quantity;
}__attribute__((packed)) B3MessagePExecutionReportForward;

typedef struct {
	B3InboundBusinessHeader business_header;
	B3MassActionType mass_action_type;
	B3MassActionScope mass_action_scope;
	B3ClOrdID cl_ord_id;
	B3ExecRestatementReason exec_restatement_reason;
	B3OrdTagID ord_tag_id;
	B3Side side;
	uint8_t __padding__;
	B3Asset asset;
	B3SecurityID security_id;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePOrderMassActionRequest;

/* Does not contains the text. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3MassActionType mass_action_type;
	B3MassActionScope mass_action_scope;
	B3ClOrdID cl_ord_id;
	B3MassActionReportID mass_action_report_id;
	B3UTCTimestampNanos transact_time;
	B3MassActionResponse mass_action_response;
	B3MassActionRejectReason mass_action_reject_reason;
	B3ExecRRVFMC exec_restatement_reason;
	B3OrdTagID ord_tag_id;
	B3Side side;
	uint8_t __padding__;
	B3Asset asset;
	B3SecurityID security_id;
	B3InvestorID investor_id;
}__attribute__((packed)) B3MessagePOrderMassActionReport;

/* Does not contain memo nor text. */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3MessageType ref_msg_type;
	uint8_t __padding__;
	B3SeqNum ref_seq_num;
	B3BusinessRejectRefID business_reject_ref_id;
	B3RejReason business_rej_reason;
}__attribute__((packed)) B3MessagePBusinessMessageReject;

typedef struct {
	B3InboundBusinessHeader business_header;
	B3SecurityReqRespID security_req_id;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3GroupSizeEncoding no_legs;
	B3Symbol leg_symbol;
	B3RatioQty leg_ratio_qty;
	B3Side leg_side;
}__attribute__((packed)) B3MessagePSecurityDefinitionRequest;

typedef struct {
	B3OutboundBusinessHeader business_header;
	uint16_t __padding__;
	B3SecurityReqRespID security_req_id;
	B3SecurityID security_id;
	B3SecurityResponseType security_response_type;
	B3SecurityStrategyType security_strategy_type;
	B3Symbol symbol;
	B3SecurityReqRespID security_response_id;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
}__attribute__((packed)) B3MessagePSecurityDefinitionResponse;

/* Does not contain quote req id, desk id nor memo. */
typedef struct {
	B3BidirectionalBusinessHeader business_header;
	B3SecurityID security_id;
	B3QuoteID quote_id;
	B3TradeID trade_id;
	B3Firm contra_broker;
	B3UTCTimestampNanos transact_time;
	B3Price price;
	B3SettlType settl_type;
	B3ExecuteUnderlyingTrade execute_underlying_trade;
	B3Quantity order_quantity;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Percentage fixed_rate;
	B3DaysToSettlement days_to_settlement;
	B3GroupSizeEncoding no_sides;
	B3Side side;
	B3Account account;
}__attribute__((packed)) B3MessagePQuoteRequest;

/* Does not contain quote req id, desk id, memo nor text. */
typedef struct {
	B3BidirectionalBusinessHeader business_header;
	B3RejReason quote_reject_reason;
	B3SecurityID security_id;
	B3QuoteID quote_id;
	B3TradeID trade_id;
	B3Firm contra_broker;
	B3UTCTimestampNanos transact_time;
	B3QuoteStatus quote_status;
	B3QuoteStatus quote_status_response_status;
	B3Account account;
	B3Side side;
	B3SettlType settl_type;
	B3Price price;
	B3Quantity order_quantity;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Trader executing_trader;
	B3Percentage fixed_rate;
	B3ExecuteUnderlyingTrade execute_underlying_trade;
	B3DaysToSettlement days_to_settlement;
}__attribute__((packed)) B3MessagePQuoteStatusReport;

/* Does not contain quote req id, desk id, nor memo. */
typedef struct {
	B3BidirectionalBusinessHeader business_header;
	B3SecurityID security_id;
	B3QuoteID quote_id;
	B3UTCTimestampNanos transact_time;
	B3Price price;
	B3Quantity order_quantity;
	B3Side side;
	B3SettlType settl_type;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Trader executing_trader;
	B3Percentage fixed_rate;
	B3ExecuteUnderlyingTrade execute_underlying_trade;
	B3DaysToSettlement days_to_settlement;
}__attribute__((packed)) B3MessagePQuote;

/* Same. */
typedef struct {
	B3BidirectionalBusinessHeader business_header;
	B3SecurityID security_id;
	B3QuoteID quote_id;
	B3Account account;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3Trader executing_trader;
}__attribute__((packed)) B3MessagePQuoteCancel;

/* Does not contain quote req id, desk id, memo nor text. */
typedef struct {
	B3BidirectionalBusinessHeader business_header;
	B3RejReason reject_reason;
	B3SecurityID security_id;
	B3QuoteID quote_id;
	B3TradeID trade_id;
	B3Firm contra_broker;
	B3UTCTimestampNanos transact_time;
	B3Trader entering_trader;
	B3SettlType settl_type;
	B3Price price;
	B3Quantity order_quantity;
	B3SenderLocation sender_location;
	B3Trader executing_trader;
	B3Percentage fixed_rate;
	B3DaysToSettlement days_to_settlement;
	B3GroupSizeEncoding no_sides;
	B3Side side;
	B3Account account;
}__attribute__((packed)) B3MessagePQuoteRequestReject;

/* Ya know. */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3PosReqID pos_req_id;
	B3SecurityID security_id;
	B3PriceOffset threshold_amount;
	B3Account account;
	B3SenderLocation sender_location;
	B3PosTransType pos_trans_type;
	B3LocalMktDate clearing_business_date;
	uint8_t is_contrary_instruction;
	B3Trader entering_trader;
	B3Quantity long_quantity;
}__attribute__((packed)) B3MessagePPositionMaintenanceRequest;

typedef struct {
	B3InboundBusinessHeader business_header;
	B3PosReqID pos_req_id;
	B3SecurityID security_id;
	B3PosReqID orig_pos_req_ref_id;
	B3PosMaintRptID pos_main_rpt_ref_id;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
}__attribute__((packed)) B3MessagePPositionMaintenanceCancelRequest;

/* ... */
typedef struct {
	B3OutboundBusinessHeader business_header;
	B3PosReqID pos_req_id;
	B3SecurityID security_id;
	B3PosMaintRptID pos_maint_rpt_id;
	B3PosTransType pos_trans_type;
	B3PosMaintAction pos_maint_action;
	B3PosMaintStatus pos_maint_status;
	B3TradeID trade_id;
	B3PosReqID orig_pos_req_ref_id;
	B3AccountType account_type;
	B3LocalMktDate clearing_business_date;
	B3PriceOffset threshold_amount;
	B3UTCTimestampNanos transact_time;
	B3Account account;
	B3SenderLocation sender_location;
	B3RejReason pos_maint_result;
	uint8_t is_contrary_instruction;
	B3GroupSizeEncoding no_positions;
	B3PosType pos_type;
	B3Quantity long_quantity;
	B3Quantity short_quantity;
}__attribute__((packed)) B3MessagePPositionMaintenanceReport;

/* ... */
typedef struct {
	B3InboundBusinessHeader business_header;
	B3AllocID alloc_id;
	B3SecurityID security_id;
	B3AllocTransType alloc_trans_type;
	B3AllocType alloc_type;
	B3AllocNoOrdersType alloc_no_orders_type;
	B3Quantity quantity;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
	B3TradeID trade_id;
	B3LocalMktDate trade_date;
	B3AllocID individual_alloc_id;
	B3Account alloc_account;
	B3Quantity alloc_quantity;
}__attribute__((packed)) B3MessagePAllocationInstruction;

typedef struct {
	B3OutboundBusinessHeader business_header;
	B3AllocID alloc_id;
	B3SecurityID security_id;
	B3AllocReportID alloc_report_id;
	B3AllocTransType alloc_trans_type;
	B3AllocReportType alloc_report_type;
	B3AllocNoOrdersType alloc_no_orders_type;
	B3RejReason alloc_rej_code;
	B3Quantity quantity;
	B3AllocStatus alloc_status;
	B3LocalMktDate trade_date;
	B3UTCTimestampNanos transact_time;
	B3Side side;
	B3SenderLocation sender_location;
	B3Trader entering_trader;
}__attribute__((packed)) B3MessagePAllocationReport;

#endif /* B3_HEADER_INCLUDED */

#pragma scalar_storage_order default
