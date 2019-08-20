/usr/lib/firebird/2.1/bin/isql -u sysdba -p SSAFirebird  -ch WIN1251 /linuxcash/cash/data/db/cash.gdb <<EOF

SET TERM ^^ ;
CREATE VIEW TMC_BY_NAME (CODE,NAME,PRICE) AS
 select substring(code from 1 for 5) code, name, max(price) price from tmc where code<>bcode group by substring(code from 1 for 5),name; ^^
SET TERM ; ^^
CREATE OR ALTER PROCEDURE ADD_TMC_ITEM (
  BCODE VarChar(20), 
  VATCODE1 Integer, 
  VATCODE2 Integer, 
  VATCODE3 Integer, 
  VATCODE4 Integer, 
  VATCODE5 Integer, 
  DCODE Integer, 
  NAME VarChar(32), 
  ARTICUL VarChar(32), 
  CQUANT Numeric(18,3), 
  MEASURE Integer, 
  PRICETYPE Integer, 
  PRICE Numeric(18,2), 
  MINPRICE Numeric(18,2), 
  VALCODE Integer, 
  QUANTDEFAULT Numeric(18,3), 
  QUANTLIMIT Numeric(18,3), 
  OSTAT SmallInt, 
  LINKS SmallInt, 
  QUANT_MODE SmallInt, 
  BCODE_MODE SmallInt, 
  OP_MODE SmallInt, 
  DEPT_MODE SmallInt, 
  PRICE_MODE SmallInt, 
  TARA_FLAG SmallInt, 
  TARA_MODE SmallInt, 
  TARA_DEFAULT VarChar(20), 
  UNIT_WEIGHT Numeric(18,3), 
  CODE VarChar(100), 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_TMC FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_TMC (ROW_GROUP, BCODE, VATCODE1, VATCODE2, VATCODE3, VATCODE4, VATCODE5, DCODE, NAME, ARTICUL,
         CQUANT, MEASURE, PRICETYPE, PRICE, MINPRICE, VALCODE, QUANTDEFAULT, QUANTLIMIT, OSTAT, LINKS, QUANT_MODE,
         BCODE_MODE, OP_MODE, DEPT_MODE, PRICE_MODE, TARA_FLAG, TARA_MODE, TARA_DEFAULT, UNIT_WEIGHT, CODE) VALUES
         (:SHADOW_GROUP, :BCODE, :VATCODE1, :VATCODE2, :VATCODE3, :VATCODE4, :VATCODE5, :DCODE, :NAME, :ARTICUL, :CQUANT,
         :MEASURE, :PRICETYPE, :PRICE, :MINPRICE, :VALCODE, :QUANTDEFAULT, :QUANTLIMIT, :OSTAT, :LINKS, :QUANT_MODE,
         :BCODE_MODE, :OP_MODE, :DEPT_MODE, :PRICE_MODE, :TARA_FLAG, :TARA_MODE, :TARA_DEFAULT, :UNIT_WEIGHT,
         :CODE);
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE ADD_CARD_ITEM (
  CARD_NUMBER Integer, 
  CARD_BCODE VarChar(60), 
  CARD_PERCENT Numeric(9,2), 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_DISCCARD FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_DISCCARD (ROW_GROUP, CARDNUMBER, NAME, RTEXT, CARDSUM, DISCASHEME, DENYCARD, NOTACCUMULATE,
                                POSITIONMODE, ISPAYCARD, ALLPOSITIONS, MSGSHEME, RCOUNT, ACDISCSCHEME,
                                CARDMODE, BONUSSUM, BONUSCODE, GROUPCODE, BIRTHDAY)
                                VALUES (:SHADOW_GROUP, :CARD_BCODE, :CARD_NUMBER, '', 0, 0, 0, 0,
                                0, 0, 0, 0, 0, cast(:CARD_PERCENT * 100 as integer),
                                0, 0, 0, 0, NULL);
end ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE ADD_KEY_ITEM (
  KEYCODE Integer, 
  COMMAND VarChar(60), 
  PARAMS VarChar(512), 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_KEYS FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_KEYS (ROW_GROUP, KEYCODE, COMMAND, PARAMS)
         VALUES (:SHADOW_GROUP, :KEYCODE, :COMMAND, :PARAMS);
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE ADD_MOL_ITEM (
  CODE VarChar(30), 
  LOGIN VarChar(20), 
  LOGINMODE SmallInt, 
  NAME VarChar(32), 
  I_PASSWORD VarChar(8), 
  I_LOCK SmallInt, 
  I_GROUP SmallInt, 
  DRAWER SmallInt, 
  I_START VarChar(8), 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_MOL FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_MOL (ROW_GROUP, CODE, LOGIN, LOGINMODE, NAME, "PASSWORD", "LOCK", "GROUP", DRAWER, "START")
         VALUES (:SHADOW_GROUP, :CODE, :LOGIN, :LOGINMODE, :NAME, :I_PASSWORD, :I_LOCK, :I_GROUP, :DRAWER, :I_START);
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE ADD_TPRICES_ITEM (
  BCODE VarChar(20), 
  I_INDEX Integer, 
  PRICE Numeric(18,2), 
  NAME VarChar(40), 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_TPRICES FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_TPRICES (ROW_GROUP, BCODE, "INDEX", PRICE, NAME)
         VALUES (:SHADOW_GROUP, :BCODE, :I_INDEX, :PRICE, :NAME);
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE GET_CASH_INFO returns (
  SHIFT Integer, 
  CASHCODE VarChar(30), 
  SHOPCODE VarChar(30), 
  ISCLOSED Integer)
AS
declare variable dt timestamp;
begin
  select cast (val as integer) from settings where name = 'CASHINFO.CASHCODE' into :cashcode;
  select cast (val as integer) from settings where name = 'CASHINFO.SHOPCODE' into :shopcode;
  select max(TIME_BEG) from cdata where CASHCODE = :cashcode into :dt;
  select shift from cdata where TIME_BEG = :dt into :shift;
  isclosed = 0;
  select 1 from rdb\$database where 
         exists (select 1 from sdata where shift = :shift) into :ISCLOSED;
  suspend;
end ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE GET_SHIFT_INFO (
  I_SHOPCODE VarChar(30), 
  I_CASHCODE VarChar(30), 
  I_SHIFT Integer)
 returns (
  SHOPCODE VarChar(30), 
  CASHCODE VarChar(30), 
  SHIFT Integer, 
  SCODE VarChar(30), 
  TIME_BEG TimeStamp, 
  TIME_END TimeStamp)
AS
BEGIN
    SELECT :I_SHOPCODE, CASHCODE, SHIFT, SCODE, TIME_BEG, TIME_END FROM
    SDATA WHERE :I_CASHCODE = CASHCODE AND :I_SHIFT = SHIFT
    INTO :SHOPCODE, :CASHCODE, :SHIFT, :SCODE, :TIME_BEG, :TIME_END; 
    SUSPEND;
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE BEGIN_SESSION (
  DATATYPE Integer, 
  INCREMENTAL Integer)
 returns (
  SESSIONID Integer)
AS
DECLARE VARIABLE UPDATE_FLAG INTEGER;
/*DECLARE VARIABLE UPDATE_TYPE INTEGER;*/
BEGIN
     SESSIONID = GEN_ID(SESSIONID_GENERATOR, 1);
     INSERT INTO UPDATE_SESSIONS (SESSIONID, DATATYPE, UPDATETYPE) VALUES (:SESSIONID, :DATATYPE, :INCREMENTAL);
     IF (DATATYPE = 1) THEN
     BEGIN
          /* яеяяхъ асдер онябъыемю йюяяхпюл */
          /* опнбепхл бнглнфмнярэ намнбкемхъ */
          SELECT FLAG FROM GET_TABLE_UPDATED('MOL') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_MOL WHERE ROW_GROUP <> (SELECT CURRENT_MOL FROM WORKING_SET);
     END ELSE IF (DATATYPE = 2) THEN
     BEGIN
          /* рнбюпш! */
          /* опнбепхл бнглнфмнярэ намнбкемхъ */
          SELECT FLAG FROM GET_TABLE_UPDATED('TMC') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_TMC WHERE ROW_GROUP <> (SELECT CURRENT_TMC FROM WORKING_SET);
     END ELSE IF (DATATYPE = 3) THEN
     BEGIN
          /* йюпрш! */
          /* опнбепхл бнглнфмнярэ намнбкемхъ */
          SELECT FLAG FROM GET_TABLE_UPDATED('DISCCARD') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          SELECT FLAG FROM GET_TABLE_UPDATED('DISCCARDACCUMULATE') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_DISCCARD WHERE ROW_GROUP <> (SELECT CURRENT_DISCCARD FROM WORKING_SET);
          DELETE FROM WORK_DISCCARDACCUMULATE WHERE ROW_GROUP <> (SELECT CURRENT_DISCCARDACCUMULATE FROM WORKING_SET);
     END ELSE IF (DATATYPE = 4) THEN
     BEGIN
         /* едхмхжш хглепемхъ */
          SELECT FLAG FROM GET_TABLE_UPDATED('UNITS') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_UNITS WHERE ROW_GROUP <> (SELECT CURRENT_UNITS FROM WORKING_SET);
     END ELSE IF (DATATYPE = 5) THEN
     BEGIN
         /* йкюбхьх */
          SELECT FLAG FROM GET_TABLE_UPDATED('KEYS') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_KEYS WHERE ROW_GROUP <> (SELECT CURRENT_KEYS FROM WORKING_SET);
     END ELSE IF (DATATYPE = 6) THEN
     BEGIN
         /* йнкнмйх жем */
          SELECT FLAG FROM GET_TABLE_UPDATED('TPRICES') INTO :UPDATE_FLAG;
          IF (UPDATE_FLAG = 1) THEN
              EXCEPTION UPDATE_IMPOSSIBLE 'опедшдсыее намнбкемхе ме опхлемемн опнцпюллни';
          /* нвхярхл ремебни яопюбнвмхй */
          DELETE FROM WORK_TPRICES WHERE ROW_GROUP <> (SELECT CURRENT_TPRICES FROM WORKING_SET);
     END
     SUSPEND;
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE COMMIT_SESSION (
  SESSIONID Integer)
AS
DECLARE VARIABLE DATATYPE INTEGER;
DECLARE VARIABLE INCREMENT INTEGER;
BEGIN
     SELECT DATATYPE, UPDATETYPE FROM UPDATE_SESSIONS WHERE SESSIONID = :SESSIONID INTO :DATATYPE, :INCREMENT;
     IF (DATATYPE = 1) THEN
     BEGIN
          /* MOL! */
          EXECUTE PROCEDURE UPD_SDW_MOL (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('MOL', 1);
     END
     ELSE IF (DATATYPE = 2) THEN
     BEGIN
          /* TMC! */
          EXECUTE PROCEDURE UPD_SDW_TMC (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('TMC', 1);
     END
     ELSE IF (DATATYPE = 3) THEN
     BEGIN
          /* DISCCARD AND DISCCARDACCUMULATE! */
          EXECUTE PROCEDURE GEN_DSCACC_BY_DSC;
          EXECUTE PROCEDURE UPD_SDW_DISCCARD (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('DISCCARD', 1);
          EXECUTE PROCEDURE UPD_SDW_DISCCARDACCUMULATE (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('DISCCARDACCUMULATE', 1);
     END
     ELSE IF (DATATYPE = 4) THEN
     BEGIN
          /* UNITS! */
          EXECUTE PROCEDURE UPD_SDW_UNITS (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('UNITS', 1);
     END
     ELSE IF (DATATYPE = 5) THEN
     BEGIN
          /* KEYS! */
          EXECUTE PROCEDURE UPD_SDW_KEYS (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('KEYS', 1);
     END
     ELSE IF (DATATYPE = 6) THEN
     BEGIN
          /* TPRICES! */
          EXECUTE PROCEDURE UPD_SDW_TPRICES (INCREMENT);
          IF (INCREMENT = 0) THEN
              EXECUTE PROCEDURE SET_TABLE_UPDATED('TPRICES', 1);
     END
     DELETE FROM UPDATE_SESSIONS WHERE SESSIONID = :SESSIONID;
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE P_UPDATE_DICTS
AS
DECLARE VARIABLE SWITCH_TO INTEGER;
DECLARE VARIABLE HAVE_UPDATE INTEGER;
BEGIN
            SELECT FLAG FROM GET_TABLE_UPDATED('DEPT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DEPT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DEPT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DEPT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('DOCSPR') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DOCSPR FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DOCSPR = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DOCSPR', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('GROUPS') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_GROUPS FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_GROUPS = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('GROUPS', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('INTERVAL') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_INTERVAL FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_INTERVAL = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('INTERVAL', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('IPOROG') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_IPOROG FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_IPOROG = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('IPOROG', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('MOL') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_MOL FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_MOL = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('MOL', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('MOP') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_MOP FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_MOP = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('MOP', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('NOTES') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_NOTES FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_NOTES = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('NOTES', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('NTLINK') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_NTLINK FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_NTLINK = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('NTLINK', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('OP') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_OP FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_OP = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('OP', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('POROG') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_POROG FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_POROG = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('POROG', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('ROSTAT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_ROSTAT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_ROSTAT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('ROSTAT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('SDABS') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_SDABS FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_SDABS = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('SDABS', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('SDPERC') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_SDPERC FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_SDPERC = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('SDPERC', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('SPINDEX') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_SPINDEX FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_SPINDEX = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('SPINDEX', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('SPRICES') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_SPRICES FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_SPRICES = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('SPRICES', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TGRLINK') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TGRLINK FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TGRLINK = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TGRLINK', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TMC') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TMC FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TMC = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TMC', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TOSTAT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TOSTAT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TOSTAT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TOSTAT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TPRICES') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TPRICES FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TPRICES = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TPRICES', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TQUICK') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TQUICK FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TQUICK = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TQUICK', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('UNITS') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_UNITS FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_UNITS = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('UNITS', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('VALUT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_VALUT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_VALUT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('VALUT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('VAT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_VAT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_VAT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('VAT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TMCEXT') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TMCEXT FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TMCEXT = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TMCEXT', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('TMCLINK') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_TMCLINK FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_TMCLINK = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('TMCLINK', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('USERS') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_USERS FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_USERS = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('USERS', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('DISCCARD') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DISCCARD FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DISCCARD = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DISCCARD', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('DISCCARDACCUMULATE') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DISCCARDACCUMULATE FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DISCCARDACCUMULATE = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DISCCARDACCUMULATE', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('DISCOUNTSAUTO') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DISCOUNTSAUTO FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DISCOUNTSAUTO = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DISCOUNTSAUTO', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('DISCOUNTSFIXED') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_DISCOUNTSFIXED FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_DISCOUNTSFIXED = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('DISCOUNTSFIXED', 0);
            END

            SELECT FLAG FROM GET_TABLE_UPDATED('KEYS') INTO :HAVE_UPDATE;
            IF (HAVE_UPDATE = 1) THEN
            BEGIN
                 SELECT CURRENT_KEYS FROM WORKING_SET INTO :SWITCH_TO;
                 IF (SWITCH_TO > 0) THEN
                     SWITCH_TO = 0;
                 ELSE
                     SWITCH_TO = 1;
                 UPDATE WORKING_SET SET CURRENT_KEYS = :SWITCH_TO;
                 EXECUTE PROCEDURE SET_TABLE_UPDATED ('KEYS', 0);
            END

END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE ADD_UNIT_ITEM (
  CODE Integer, 
  NAME VarChar(8), 
  FLAG SmallInt, 
  SESSIONID Integer)
AS
DECLARE VARIABLE SHADOW_GROUP INTEGER;
BEGIN
  SELECT CURRENT_UNITS FROM WORKING_SET INTO :SHADOW_GROUP;
  IF (SHADOW_GROUP = 1) THEN
      SHADOW_GROUP = 0;
  ELSE
      SHADOW_GROUP = 1;
  INSERT INTO WORK_UNITS (ROW_GROUP, CODE, NAME, FLAG)
         VALUES (:SHADOW_GROUP, :CODE, :NAME, :FLAG);
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE GET_SHIFT_SALES (
  I_SHOPCODE Integer, 
  I_CASHCODE Integer, 
  I_SHIFT Integer)
 returns (
  SHOPCODE Integer, 
  CASHCODE Integer, 
  SHIFT Integer, 
  CHECKNUMBER Integer, 
  POSNUMBER Integer, 
  DATETIME TimeStamp, 
  BCODE VarChar(20), 
  CODE VarChar(100), 
  QUANTITY Numeric(18,3), 
  PRICE Numeric(18,2), 
  SUMM Numeric(18,2), 
  SCODE Integer, 
  DIRECTION Integer, 
  OPCODE Integer, 
  CREDITCARD VarChar(100), 
  DISCCARD VarChar(100), 
  LINK Integer)
AS
DECLARE VARIABLE MYOPCODE INTEGER;
DECLARE VARIABLE VALCODE INTEGER;
BEGIN
  FOR SELECT :I_SHOPCODE, CASHCODE, SHIFT, CHECKNUM, POSNUM, TTIME, BCODE, CODE, CQUANT,
             PRICE, SUMB, SCODE, OPCODE FROM TDATA
             WHERE :I_SHIFT = TDATA.SHIFT AND :I_CASHCODE = TDATA.CASHCODE
             INTO :SHOPCODE, :CASHCODE, :SHIFT, :CHECKNUMBER, :POSNUMBER, :DATETIME, :BCODE, :CODE, :QUANTITY,
             :PRICE, :SUMM, :SCODE, :MYOPCODE DO
  BEGIN
       IF (BCODE = 'sumdept') THEN
           BCODE = '';
       DIRECTION = 0;
       IF (MYOPCODE = 50 OR MYOPCODE = 52 OR MYOPCODE = 1000) THEN
           DIRECTION = 1;
       VALCODE = 1;
       SELECT FIRST 1 VALCODE, CARDNUM FROM MDATA
              WHERE CASHCODE = :CASHCODE AND SHIFT = :SHIFT AND CHECKNUM = :CHECKNUMBER
              INTO :VALCODE, :CREDITCARD;
       IF (MYOPCODE = 50 OR MYOPCODE = 52 OR MYOPCODE = 1000) THEN
       BEGIN
            IF (VALCODE = 1) THEN
                OPCODE = 1;
            ELSE
                OPCODE = 5;
       END
       ELSE
       BEGIN
            IF (VALCODE = 1) THEN
                OPCODE = 0;
            ELSE
                OPCODE = 4;
       END
       DISCCARD = '';
       SELECT FIRST 1 CARDNUMBER FROM DISCDATA
              WHERE CASHCODE = :CASHCODE AND CHECKNUM = :CHECKNUMBER
                    AND SHIFT = :SHIFT INTO :DISCCARD;
       LINK = 0;
       SUSPEND;
  END
END ^^
SET TERM ; ^^
SET TERM ^^ ;
CREATE OR ALTER PROCEDURE GET_SHIFT_STORNO (
  I_SHOPCODE Integer, 
  I_CASHCODE Integer, 
  I_SHIFT Integer)
 returns (
  SHOPCODE Integer, 
  CASHCODE Integer, 
  SHIFT Integer, 
  CHECKNUMBER Integer, 
  POSNUMBER Integer, 
  DATETIME TimeStamp, 
  BCODE VarChar(20), 
  CODE VarChar(100), 
  QUANTITY Numeric(18,3), 
  PRICE Numeric(18,2), 
  SUMM Numeric(18,2), 
  SCODE Integer, 
  DIRECTION Integer, 
  OPCODE Integer, 
  CREDITCARD VarChar(100), 
  DISCCARD VarChar(100), 
  LINK Integer)
AS
DECLARE VARIABLE MYOPCODE INTEGER;
DECLARE VARIABLE VALCODE INTEGER;
BEGIN
  FOR SELECT :I_SHOPCODE, CASHCODE, SHIFT, CHECKNUM, POSNUM, TSTIME, BCODE, CODE, CQUANT,
             PRICE, SUMB, SCODE, OPCODE FROM TSDATA
             WHERE :I_SHIFT = TSDATA.SHIFT AND :I_CASHCODE = TSDATA.CASHCODE
             INTO :SHOPCODE, :CASHCODE, :SHIFT, :CHECKNUMBER, :POSNUMBER, :DATETIME, :BCODE, :CODE, :QUANTITY,
             :PRICE, :SUMM, :SCODE, :MYOPCODE DO
  BEGIN
       IF (BCODE = 'sumdept') THEN
           BCODE = '';
       DIRECTION = 0;
       IF (MYOPCODE = 50 OR MYOPCODE = 52 OR MYOPCODE = 1000) THEN
           DIRECTION = 1;
       VALCODE = 1;
       SELECT FIRST 1 VALCODE, CARDNUM FROM MDATA
              WHERE CASHCODE = :CASHCODE AND SHIFT = :SHIFT AND CHECKNUM = :CHECKNUMBER
              INTO :VALCODE, :CREDITCARD;
       IF (MYOPCODE = 50 OR MYOPCODE = 52 OR MYOPCODE = 1000) THEN
       BEGIN
            IF (VALCODE = 1) THEN
                OPCODE = 1;
            ELSE
                OPCODE = 5;
       END
       ELSE
       BEGIN
            IF (VALCODE = 1) THEN
                OPCODE = 0;
            ELSE
                OPCODE = 4;
       END
       CREDITCARD = '';
       DISCCARD = '';
       SELECT FIRST 1 CARDNUMBER FROM DISCDATA
              WHERE CASHCODE = :CASHCODE AND CHECKNUM = :CHECKNUMBER
                    AND SHIFT = :SHIFT INTO :DISCCARD;
       LINK = 0;
       SUSPEND;
  END
END ^^
SET TERM ; ^^

EOF
