try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET


def get_named_elements_from_ipf_file(ipf_file, names_to_search, value_type):
    """
    Args:
        ipf_file: The path to the IPF
        names: A list of search names
    Returns: A map of the search names and the found information
    """
    output = {}
    number_of_elements_to_search = len(names_to_search)
    for event, element in ET.iterparse(ipf_file):
        if element.tag == "parameter" and "name" in element.keys():
            if element.get("name") in names_to_search:
                sub_element = element.find("value")
                value = sub_element.get("val")
                output.update({element.get("name"): value_type(value)})
                element.clear()
                if number_of_elements_to_search == len(output):
                    break
    return output


def get_monitor_names_from_idf_file(idf_file):
    def get_tag(tag_in):
        return "{http://www.mantidproject.org/IDF/1.0}" + tag_in
    output = {}
    tag = "idlist"
    idname = "idname"
    id_tag = "id"
    for event, element in ET.iterparse(idf_file):
        if element.tag == get_tag(tag) and idname in element.keys():
            name = element.get(idname)
            if "monitor" in name:
                sub_element = element.find(get_tag(id_tag))
                # We can have two situations here:
                # 1. either monitors are separate, e.g. <idlist idname="monitor1"> <id val="1" /> </idlist>, ..
                # 2. or in a range, e.g. <idlist idname="monitors"> <id start="1" end="8" /> </idlist>
                val = sub_element.get("val")
                start = sub_element.get("start")
                end = sub_element.get("end")
                if val:
                    output.update({int(val): name})
                    element.clear()
                elif start and end:
                    for index in range(int(start), int(end) + 1):
                        monitor_id = "monitor" + str(index)
                        output.update({index: monitor_id})
                    element.clear()
                else:
                    continue

    return output