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

/*
 * Most types founded in headers and etc.
 */

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

typedef uint8_t B3ChannelID;
typedef uint16_t B3BlockLength;
typedef uint16_t B3SchemaID;
typedef uint16_t B3SchemaVersion;
typedef uint16_t B3Version;

/*
 * Most types found in messages.
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

typedef uint8_t B3SecurityIDSource;
#define B3_SECURITY_ID_SOURCE_ISIN ((uint8_t) '4')
#define B3_SECURITY_ID_SOURCE_EXCHANGE_SYMBOL ((uint8_t) '8')

typedef uint8_t B3SecurityUpdateAction;
#define B3_SECURITY_UPDATE_ADD ((uint8_t) 'A')
#define B3_SECURITY_UPDATE_DELETE ((uint8_t) 'D')
#define B3_SECURITY_UPDATE_MODIFY ((uint8_t) 'M')

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
typedef uint8_t B3NumInGroup;
typedef uint8_t B3Reserved;
typedef uint8_t B3Week;

typedef uint16_t B3blockLength;
typedef uint16_t B3EncodingType;
typedef uint16_t B3Utf8StringLength;
typedef uint16_t B3LocalMktDate;
typedef uint16_t B3MessageLength;
typedef uint16_t B3PacketLength;
typedef uint16_t B3SequenceVersion;
typedef uint16_t B3SettlType;
typedef uint16_t B3Year;

typedef int32_t B3LocalMktDate32;

typedef uint32_t B3FirmOptional;
typedef uint32_t B3MDEntryPositionNo;
typedef uint32_t B3NumberOfTrades;
typedef uint32_t B3RptSeq;
typedef uint32_t B3SeqNum;
typedef uint32_t B3SequenceNumber;
typedef uint32_t B3TradeID;

typedef int64_t B3Quantity;
typedef int64_t B3QuantityVolume;
typedef int64_t B3Fixed8;
typedef int64_t B3Price;
typedef int64_t B3RatioQty;

typedef uint64_t B3ClearingHouseID;
typedef uint64_t B3NewsID;
typedef uint64_t B3OrderID;
typedef uint64_t B3SecurityID;
typedef uint64_t B3SendingTime;
typedef uint64_t B3Time;
typedef uint64_t B3UTCTimestampSeconds;
typedef uint64_t B3UTCTimestampNanos;

typedef struct {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
}__attribute__((packed)) B3MaturityMonthYear;

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
	B3ChannelID channel_id;
	B3Reserved reserved;
	B3SequenceVersion sequence_version;
	B3SequenceNumber sequence_number;
	B3SendingTime sending_time;
} __attribute__((packed)) B3PacketHeader;

typedef struct {
	B3MessageLength length;
	B3EncodingType encoding;
	B3BlockLength block_length;
	B3TemplateID template_id;
	B3SchemaID schema_id;
	B3SchemaVersion schema_version;
} __attribute__((packed)) B3MessageHeader;

/*
 * Messages.
 */

typedef struct {
	B3SeqNum next_sequence_number;
}__attribute__((packed)) B3Sequence;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3SecurityTradingStatus security_trading_status;
	B3SecurityTradingEvent security_trading_event;
	B3LocalMktDate trade_date;
	uint16_t __padding;
	B3UTCTimestampNanos trade_session_open_time;
	B3UTCTimestampNanos transaction_time;
	B3RptSeq rpt_seq;
}__attribute__((packed)) B3SecurityStatus;

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
}__attribute__((packed)) B3News;

typedef struct {
	B3SecurityID security_id;
	B3MatchEventIndicator match_event_indicator;
	uint16_t __padding;
	uint8_t __padding2;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3EmptyBook;

typedef struct {
	B3SecurityGroup security_group;
	uint32_t __padding;
	uint8_t __padding2;
	B3MatchEventIndicator match_event_indicator;
	B3TradingSessionID trading_session_id;
	B3TradingSessionSubID trading_session_subid;
	B3SecurityTradingEvent security_trading_event;
	B3LocalMktDate trade_date;
	uint16_t __padding3;
	B3UTCTimestampNanos trade_session_open_time;
	B3UTCTimestampNanos transaction_time;
}__attribute__((packed)) B3SecurityGroupPhase;

typedef struct {
	B3MatchEventIndicator match_event_indicator;
	uint16_t __padding;
	uint8_t __padding2;
	B3UTCTimestampNanos md_entry_timestamp;
}__attribute__((packed)) B3ChannelReset;

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
}__attribute__((packed)) B3SecurityDefinition;

#ifdef B3_IMPLEMENTATION
#undef B3_IMPLEMENTATION

#endif /* B3_IMPLEMENTATION */

#endif /* B3_HEADER_INCLUDED */

#pragma scalar_storage_order default