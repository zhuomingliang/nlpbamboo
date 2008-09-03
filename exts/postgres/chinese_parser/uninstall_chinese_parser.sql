-- Adjust this setting to control where the objects get dropped.
SET search_path = pg_catalog;

DROP TEXT SEARCH PARSER chineseparser;

DROP FUNCTION chineseprs_start(internal, int4);

DROP FUNCTION chineseprs_getlexeme(internal, internal, internal);

DROP FUNCTION chineseprs_end(internal);

DROP FUNCTION chineseprs_lextype(internal);
