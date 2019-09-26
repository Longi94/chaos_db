.mode csv
.sep |
.print "Importing region..."
.import ../tpch/tables/region.tbl region
.print "Importing nation..."
.import ../tpch/tables/nation.tbl nation
.print "Importing supplier..."
.import ../tpch/tables/supplier.tbl supplier
.print "Importing customer..."
.import ../tpch/tables/customer.tbl customer
.print "Importing part..."
.import ../tpch/tables/part.tbl part
.print "Importing partsupp..."
.import ../tpch/tables/partsupp.tbl partsupp
.print "Importing orders..."
.import ../tpch/tables/orders.tbl orders
.print "Importing lineitem..."
.import ../tpch/tables/lineitem.tbl lineitem
