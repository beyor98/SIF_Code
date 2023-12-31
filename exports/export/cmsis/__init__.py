import os
from os.path import sep, join, exists
from itertools import groupby
from xml.etree.ElementTree import Element, tostring
import ntpath
import re
import json

from exports.arm_pack_manager import Cache
from exports.targets import TARGET_MAP
from exports.export.exporters import Exporter, TargetNotSupportedException

class fileCMSIS():
    """CMSIS file class.

    Encapsulates information necessary for files in cpdsc project file"""
    file_types = {'.cpp': 'sourceCpp', '.c': 'sourceC', '.s': 'sourceAsm',
                  '.obj': 'object', '.o': 'object', '.lib': 'library',
                  '.ar': 'linkerScript', '.h': 'header', '.sct': 'linkerScript'}

    def __init__(self, loc, name):
        #print loc
        _, ext = os.path.splitext(loc)
        self.type = self.file_types[ext.lower()]
        self.loc = loc
        self.name = name


class DeviceCMSIS():
    """CMSIS Device class

    Encapsulates target information retrieved by arm-pack-manager"""

    CACHE = Cache(True, False)
    def __init__(self, target):
        target_info = self.check_supported(target)
        if not target_info:
            raise TargetNotSupportedException("Target not supported in CMSIS pack")
        self.url = target_info['pdsc_file']
        self.pack_url, self.pack_id = ntpath.split(self.url)
        self.dname = target_info["_cpu_name"]
        self.core = target_info["_core"]
        self.dfpu = target_info['processor']['fpu']
        self.debug, self.dvendor = self.vendor_debug(target_info['vendor'])
        self.dendian = target_info['processor'].get('endianness','Little-endian')
        self.debug_svd = target_info.get('debug', '')
        self.compile_header = target_info['compile']['header']
        self.target_info = target_info

    @staticmethod
    def check_supported(target):
        t = TARGET_MAP[target]
        try:
            cpu_name = t.device_name
            target_info = DeviceCMSIS.CACHE.index[cpu_name]
        # Target does not have device name or pdsc file
        except:
            try:
                # Try to find the core as a generic CMSIS target
                cpu_name = DeviceCMSIS.cpu_cmsis(t.core)
                target_info = DeviceCMSIS.CACHE.index[cpu_name]
            except:
                return False
        target_info["_cpu_name"] = cpu_name
        target_info["_core"] = t.core
        return target_info

    def vendor_debug(self, vendor):
        """Reads the vendor from a PDSC <dvendor> tag.
        This tag contains some additional numeric information that is meaningless
        for our purposes, so we use a regex to filter.

        Positional arguments:
        Vendor - information in <dvendor> tag scraped from ArmPackManager

        Returns a tuple of (debugger, vendor)
        """
        reg = "([\w\s]+):?\d*?"
        m = re.search(reg, vendor)
        vendor_match = m.group(1) if m else None
        debug_map ={
            'STMicroelectronics':'ST-Link',
            'Silicon Labs':'J-LINK',
            'Nuvoton':'NULink'
        }
        return debug_map.get(vendor_match, "CMSIS-DAP"), vendor_match

    @staticmethod
    def cpu_cmsis(cpu):
        """
        Transforms information from targets.json to the way the generic cores are named
        in CMSIS PDSC files.
        Ex:
        Cortex-M4F => ARMCM4_FP, Cortex-M0+ => ARMCM0P
        Returns formatted CPU
        """
        cpu = cpu.replace("Cortex-","ARMC")
        cpu = cpu.replace("+","P")
        cpu = cpu.replace("F","_FP")
        return cpu


class CMSIS(Exporter):
    NAME = 'cmsis'
    TOOLCHAIN = 'ARM'

    @classmethod
    def is_target_supported(cls, target_name):
        target = TARGET_MAP[target_name]
        return cls.TOOLCHAIN in target.supported_toolchains

    def make_key(self, src):
        """turn a source file into its group name"""
        key = src.name.split(sep)[0]
        if key == ".":
            key = os.path.basename(os.path.realpath(self.export_dir))
        return key

    def group_project_files(self, sources, root_element):
        """Recursively group the source files by their encompassing directory"""

        data = sorted(sources, key=self.make_key)
        for group, files in groupby(data, self.make_key):
            new_srcs = []
            for f in list(files):
                spl = f.name.split(sep)
                if len(spl) <= 2:
                    file_element = Element('file',
                                           attrib={
                                               'category':f.type,
                                               'name': f.loc})
                    root_element.append(file_element)
                else:
                    f.name = os.path.join(*spl[1:])
                    new_srcs.append(f)
            if new_srcs:
                group_element = Element('group',attrib={'name':group})
                root_element.append(self.group_project_files(new_srcs,
                                                        group_element))
        return root_element

    def generate(self):
        srcs = self.resources.headers + self.resources.s_sources + \
               self.resources.c_sources + self.resources.cpp_sources + \
               self.resources.objects + self.resources.libraries + \
               [self.resources.linker_script]
        srcs = [fileCMSIS(src, src) for src in srcs if src]
        ctx = {
            'name': self.project_name,
            'project_files': tostring(self.group_project_files(srcs, Element('files'))),
            'device': DeviceCMSIS(self.target),
            'date': ''
        }
        self.gen_file('cmsis/cpdsc.tmpl', ctx, 'project.cpdsc')
