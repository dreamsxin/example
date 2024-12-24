```go
package main

import (
	"github.com/go-mysql-org/go-mysql/canal"
	"github.com/go-mysql-org/go-mysql/mysql"
	"github.com/go-mysql-org/go-mysql/replication"
	"github.com/siddontang/go-log/log"
)


type masterInfo struct {
	sync.RWMutex

	Name string `json:"bin_name"`
	Pos  uint32 `json:"bin_pos"`

	filePath     string
	lastSaveTime time.Time
}

func loadMasterInfo(dataDir string) (*masterInfo, error) {
	var m masterInfo

	if len(dataDir) == 0 {
		return &m, nil
	}

	m.filePath = path.Join(dataDir, "master.info")
	m.lastSaveTime = time.Now()

	if err := os.MkdirAll(dataDir, 0755); err != nil {
		return nil, err
	}

	data, err := os.ReadFile(m.filePath)
	if err != nil && !os.IsNotExist(err) {
		return nil, err
	} else if os.IsNotExist(err) {
		return &m, nil
	}

	err = json.Unmarshal(data, &m)
	return &m, err
}

func (m *masterInfo) Save(pos mysql.Position) error {
	log.Printf("save position %s\n", pos)

	m.Lock()
	defer m.Unlock()

	m.Name = pos.Name
	m.Pos = pos.Pos

	if len(m.filePath) == 0 {
		return nil
	}

	n := time.Now()
	if n.Sub(m.lastSaveTime) < time.Second {
		return nil
	}

	m.lastSaveTime = n

	buf, err := json.Marshal(m)

	if err != nil {
		log.Printf("canal save master info to file %s err %v\n", m.filePath, err)
		return err
	}

	return os.WriteFile(m.filePath, buf, 0644)
}

func (m *masterInfo) Position() mysql.Position {
	m.RLock()
	defer m.RUnlock()

	return mysql.Position{
		Name: m.Name,
		Pos:  m.Pos,
	}
}

func (m *masterInfo) Close() error {
	pos := m.Position()

	return m.Save(pos)
}

type MyEventHandler struct {
	canal.DummyEventHandler
}

var master *masterInfo

func init() {
	var err error
	master, err = loadMasterInfo("./log")
	if err != nil {
		log.Panicln(err)
		return
	}
}

func (*MyEventHandler) OnRotate(header *replication.EventHeader, e *replication.RotateEvent) error {
	pos := mysql.Position{
		Name: string(e.NextLogName),
		Pos:  uint32(e.Position),
	}
	master.Save(pos)
	return nil
}

func (*MyEventHandler) OnTableChanged(header *replication.EventHeader, schema, table string) error {

	return nil
}

func (*MyEventHandler) OnDDL(header *replication.EventHeader, nextPos mysql.Position, _ *replication.QueryEvent) error {

	master.Save(nextPos)
	return nil
}

func (*MyEventHandler) OnXID(header *replication.EventHeader, nextPos mysql.Position) error {
	master.Save(nextPos)
	return nil
}

func (*MyEventHandler) OnRow(e *canal.RowsEvent) error {

	log.Infof("table %s action %s rows %v\n", e.Table.Name, e.Action, len(e.Rows))
	switch e.Action {
	case canal.UpdateAction:
	case canal.InsertAction:
	case canal.DeleteAction:

	}

	return nil
}

func (h *MyEventHandler) OnPosSynced(header *replication.EventHeader, pos mysql.Position, set mysql.GTIDSet, force bool) error {

	log.Infof("---------------pos %v\n", pos)
	return nil
}

func (h *MyEventHandler) String() string {
	return "MyEventHandler"
}

func main() {
	cfg := canal.NewDefaultConfig()
	cfg.Addr = "rds-yunlogin-public.mysql.rds.aliyuncs.com:3306"
	cfg.User = "admin1_go"
	cfg.Password = "0sw57ZPHKB05lfJi"

	cfg.IncludeTableRegex = []string{"yl_device.*", "yl_shop.*"}
	cfg.Dump.Databases = []string{"yl_device", "yl_shop"}
	//cfg.Dump.Tables = []string{}
	cfg.Dump.ExecutionPath = ""

	c, err := canal.NewCanal(cfg)
	if err != nil {
		log.Fatal(err)
	}

	//c.AddDumpDatabases(cfg.Dump.Databases...)
	//c.AddDumpTables(cfg.Dump.Databases[0], cfg.Dump.Tables...)

	lastPos := master.Position()
	// Register a handler to handle RowsEvent
	c.SetEventHandler(&MyEventHandler{})
	c.RunFrom(lastPos)
	master.Close()
}
```
