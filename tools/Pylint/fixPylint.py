import os
import re
import argparse

def fixBadIndent(line,indent):
    err=indent.replace(' Bad indentation. Found ','').replace('(bad-indentation)','').split('spaces, expected')
    bad=int(err[0])
    good=int(err[1])
    return line.replace(' '*bad,' '*good)

def fixParens(fname,err,errlines):
    f=open(fname,'r')
    content=f.readlines()
    f.close()
    newcontent=content
    for i,error in zip(errlines,err):
        if error.find('(superfluous-parens)')!=-1:
            kwd=error.split("'")[1]
            pattern=re.compile(kwd+'(?P<spaces>\ *)\((?P<content>.*)\)')
            match=pattern.search(content[i])
            d=match.groupdict()
            newcontent[i]=content[i].replace(kwd+d['spaces']+'('+d['content']+')',kwd+' '+d['content'])
    f=open(fname,'w')
    f.write(''.join(newcontent))
    f.close()

def fixSeveralErrors(fname,err,errlines):
    f=open(fname,'r')
    content=f.readlines()
    f.close()
    newcontent=content
    for i,error in zip(errlines,err):
        if error.find('Bad indentation')!=-1:
            newcontent[i]=fixBadIndent(content[i],error)
        if error.find('missing-final-newline')!=-1:
            newcontent[-1]+='\n'
        if error.find('mixed-indentation')!=-1:
            newcontent[i]=content[i].replace('\t', '    ')
        if error.find('trailing-whitespace')!=-1:
            newcontent[i]=content[i].rstrip()+'\n'
        if error.find('Unnecessary semicolon')!=-1:
            newcontent[i]=content[i].replace(';','')
    f=open(fname,'w')
    f.write(''.join(newcontent))
    f.close()

def addIgnoreStatement(fname,err):
    f=open(fname,'r')
    content=f.readlines()
    f.close()
    addNoInit=False
    addInvalidName=False
    ignore=[]
    for error in err:
        if error.find('(invalid-name)')!=-1 and 'invalid-name' not in ignore:
            ignore.append('invalid-name')
        if error.find('(no-init)')!=-1 and 'no-init' not in ignore:
            ignore.append('no-init')
        if error.find('(too-many-lines)')!=-1 and 'too-many-lines' not in ignore:
            ignore.append('too-many-lines')
    if len(ignore)!=0:
        f=open(fname,'w')
        f.write("#pylint: disable="+','.join(ignore)+'\n')
        f.write(''.join(content))
        f.close()

def generate_choices():
    ch=['simple','parentheses','add_ignores']
    ch_help=['simple - fixes the following warning: bad-indentation, missing-final-newline, mixed_indentation, trailing-whitespace, unnecesary-semicolon',
             'parentheses - fixes superfluous-parens warning',
             'add_ignores - adds ignore statemets at the beginning of each file to ignore invalid-name, no-init, too-many-lines']
    chhelp="The possible choices supported are:\n\t"+'\n\t'.join(ch_help)
    return (ch,chhelp)

if __name__=='__main__':
    choices,choices_help=generate_choices()
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter,
                                     epilog=choices_help,
                                     description='Fix some pylint warnings. It is STRONGLY RECOMMENDED to rerun the pylintcheck between fixes')
    parser.add_argument('-fix','--fix', default='simple',
                        choices=choices,
                        help='Select things to fix (default: simple). \nSee the choices options below.')
    parser.add_argument('Filename', type=file, help='The output from "make pylint"')
    parser.add_argument('-v','--version', action='version', version='%(prog)s 1.0')

    #read pylint.log
    args = parser.parse_args()
    oneline=args.Filename.read()
    args.Filename.close()
    #ignore everything up to the first error
    oneline=oneline[oneline.find('****'):]
    content=oneline.split('\n')

    fileindex=content.index('Checks of the following modules FAILED:')
    files=[f.strip() for f in content[fileindex+1:-1]]
    filenumber=0
    linenumber=0
    prevFile=True


    while linenumber<fileindex:
        filename=files[filenumber]
        line=content[linenumber]
        #find a file
        if line.find('****')==0 and line.find('PyChop')==-1:
            modname=line.split('************* Module ')[1]
            if prevFile:
                if os.path.isfile(filename):
                    correct_filename=filename
                    filenumber+=1
                    prevFile=True
                else:
                    prevFile=False
                    modname=modname.replace('.','/')
                    firstslash=modname.find('/')
                    if firstslash==-1:
                        correct_filename=filename+'/__init__.py'
                    else:
                        correct_filename=filename+modname[firstslash:]+'.py'
            else:
                lastmodulename=os.path.split(filename)[1]
                modbase=modname.split('.')[0]
                if lastmodulename==modbase: #still in the same module
                    modname=modname.replace('.','/')
                    firstslash=modname.find('/')
                    if firstslash==-1:
                        correct_filename=filename+'/__init__.py'
                    else:
                        correct_filename=filename+modname[firstslash:]+'.py'
                else: #go to the next file or module
                    filenumber+=1
                    filename=files[filenumber]
                    prevFile=True
                    if os.path.isfile(filename):
                        correct_filename=filename
                        filenumber+=1
                        prevFile=True
                    else:
                        prevFile=False
                        modname=modname.replace('.','/')
                        firstslash=modname.find('/')
                        if firstslash==-1:
                            correct_filename=filename+'/__init__.py'
                        else:
                            correct_filename=filename+modname[firstslash:]+'.py'
            #process it
            j=1
            errors=[]
            errorlines=[]
            while linenumber+j<fileindex and content[linenumber+j].find('****')==-1:
                err=content[linenumber+j]
                if len(err)>0 and err[0] in ['C','W','E'] and err[1]==':':
                    lineNumber=int(err.split(':')[1].split(',')[0])-1
                    errors.append(err.split(':')[2])
                    errorlines.append(lineNumber)
                j+=1
            if args.fix=='simple':
                tryToFixError(correct_filename,errors,errorlines)
            elif args.fix=='add_ignores':
                addIgnoreStatement(correct_filename,errors)
            elif args.fix=='parentheses':
                fixParens(correct_filename,errors,errorlines)
            linenumber+=j-1
        else:
            linenumber+=1







