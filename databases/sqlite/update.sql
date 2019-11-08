.mode csv
.sep |
.print "Importing orders.tbl.u1..."
.import {DIR}/../tpch/updates/orders.tbl.u1 orders
.print "Importing lineitem.tbl.u1..."
.import {DIR}/../tpch/updates/lineitem.tbl.u1 lineitem
