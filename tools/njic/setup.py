import os
import glob
from setuptools import setup, find_packages


with open(os.path.join(os.path.dirname(__file__), 'README.rst')) as readme:
    README = readme.read()


# allow setup.py to be run from any path
os.chdir(os.path.normpath(os.path.join(os.path.abspath(__file__), os.pardir)))


setup(
    name='nji',
    version='0.1.0',
    packages=find_packages('src'),
    package_dir={'': 'src'},
    package_data={'nji': ['templates/*']},
    scripts=glob.glob('scripts/*'),
    install_requires=['jinja2'],
    include_package_data=True,
    license='BSD License',
    description='Native Java Interface code generator.',
    long_description=README,
    author='greenbender',
    author_email='byron.platt@gmail.com',
    url='https://github.com/greenbender/libnji',
    zip_safe=False,
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.7',
    ],
)
