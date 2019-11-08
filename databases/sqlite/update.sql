.mode csv
.sep |
.print "Importing orders.tbl.u1..."
.import ../tpch/updates/orders.tbl.u1 orders
.print "Importing lineitem.tbl.u1..."
.import ../tpch/updates/lineitem.tbl.u1 lineitem
