/usr/lib/firebird/2.1/bin/isql -u sysdba -p SSAFirebird  -ch WIN1251 /linuxcash/cash/data/db/cash.gdb <<EOF

SET TERM ^^ ;
CREATE VIEW TMC_BY_NAME (CODE,NAME,PRICE) AS
 select substring(code from 1 for 5) code, name, max(price) price from tmc where code<>bcode group by substring(code from 1 for 5),name; ^^
SET TERM ; ^^

EOF
