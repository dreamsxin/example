package job

import (
	"github.com/go-mysql-org/go-mysql/canal"
	"github.com/go-mysql-org/go-mysql/mysql"
	"github.com/go-mysql-org/go-mysql/replication"
	"github.com/siddontang/go-log/log"
)

type MyEventHandler struct {
	canal.DummyEventHandler
}

func (h *MyEventHandler) OnRow(e *canal.RowsEvent) error {
	//UpdateAction = "update"
	//InsertAction = "insert"
	//DeleteAction = "delete"
	log.Infof("table %s action %s rows %v\n", e.Table.Name, e.Action, len(e.Rows))
	switch e.Action {
	case canal.UpdateAction:
	case canal.InsertAction:
	case canal.DeleteAction:

	}
	//for e.Rows
	return nil
}

var lastPos mysql.Position

func (h *MyEventHandler) OnPosSynced(header *replication.EventHeader, pos mysql.Position, set mysql.GTIDSet, force bool) error {
	lastPos = pos
	log.Infof("---------------pos %v\n", pos)
	return nil
}

func (h *MyEventHandler) String() string {
	return "MyEventHandler"
}

func SyncMysql() {
	cfg := canal.NewDefaultConfig()
	cfg.Addr = "127.0.0.1:3306"
	cfg.User = "root"
	cfg.Password = "root"
	// We only care table canal_test in test db
	cfg.Dump.TableDB = "test"
	cfg.Dump.Tables = []string{"helps"}
	cfg.Dump.ExecutionPath = "" //"D:\\Program Files\\MySQL\\MySQL Server 8.2\\bin\\mysqldump.exe"

	c, err := canal.NewCanal(cfg)
	if err != nil {
		log.Fatal(err)
	}
	lastPos = mysql.Position{Name: "YC-20231127CPCD-bin.000037", Pos: 272818}
	// Register a handler to handle RowsEvent
	c.SetEventHandler(&MyEventHandler{})
	c.RunFrom(lastPos)

	// Start canal
	//c.Run()
}
