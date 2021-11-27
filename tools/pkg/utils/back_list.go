package utils

type StringBackList struct {
	List []string
}

func (sl *StringBackList) Append(v string) {
	var newList []string
	for _, e := range sl.List {
		if e != v {
			newList = append(newList, e)
		}
	}
	newList = append(newList, v)
	sl.List = newList
}
